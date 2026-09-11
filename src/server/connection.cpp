#include "server/connection.hpp"

#include <chrono>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

namespace server {

Connection::Connection(
    net::Socket socket,
    std::shared_ptr<tls::TlsContext> tls_context,
    http::ParseLimits http_limits,
    std::uint32_t request_timeout_seconds,
    std::uint32_t keep_alive_timeout_seconds)
    : socket_(std::move(socket)),
      tls_context_(std::move(tls_context)),
      parser_(http_limits),
      request_timeout_seconds_(request_timeout_seconds),
      keep_alive_timeout_seconds_(keep_alive_timeout_seconds),
      tls_enabled_(tls_context_ != nullptr) {}

Connection::~Connection() { close(); }

bool Connection::handshake() {
    if (!tls_enabled_ || !tls_context_) {
        handshake_complete_ = true;
        return true;
    }
    tls_connection_ = std::make_unique<tls::TlsConnection>(socket_, tls_context_);
    handshake_complete_ = tls_connection_->handshake();
    return handshake_complete_;
}

void Connection::run(const http::RequestHandler& handler) {
    while (open_) {
        auto request = read_request();
        if (!request.has_value()) {
            break;
        }

        auto response = handler(*request);
        if (!send_response(response)) {
            break;
        }

        if (!should_keep_alive(*request)) {
            break;
        }
    }
    close();
}

void Connection::close() noexcept {
    open_ = false;
    if (tls_connection_) {
        tls_connection_->close();
    }
    socket_.close();
}

bool Connection::is_open() const noexcept { return open_ && socket_.valid(); }

bool Connection::is_tls() const noexcept { return tls_enabled_; }

std::ptrdiff_t Connection::read(char* buffer, std::size_t size) {
    if (!socket_.valid()) {
        return -1;
    }
    if (tls_enabled_ && tls_connection_) {
        std::span<std::byte> view(reinterpret_cast<std::byte*>(buffer), size);
        return tls_connection_->read(view);
    }
    return socket_.receive(std::span<std::byte>(reinterpret_cast<std::byte*>(buffer), size));
}

std::ptrdiff_t Connection::write(const char* data, std::size_t size) {
    if (!socket_.valid()) {
        return -1;
    }
    if (tls_enabled_ && tls_connection_) {
        const std::span<const std::byte> view(reinterpret_cast<const std::byte*>(data), size);
        return tls_connection_->write(view);
    }
    return socket_.send(std::span<const std::byte>(reinterpret_cast<const std::byte*>(data), size));
}

std::optional<http::HttpRequest> Connection::read_request() {
    char buffer[4096];
    for (;;) {
        const auto result = parser_.parse(read_buffer_);
        if (result.success()) {
            read_buffer_.erase(0, result.consumed);
            return result.request;
        }
        if (result.error != http::ParseError::IncompleteRequest) {
            throw std::runtime_error("Failed to parse HTTP request");
        }

        const auto count = read(buffer, sizeof(buffer));
        if (count <= 0) {
            return std::nullopt;
        }
        read_buffer_.append(buffer, static_cast<std::size_t>(count));
    }
}

bool Connection::send_response(const http::HttpResponse& response) {
    const auto payload = response.serialize();
    if (write(payload.data(), payload.size()) < 0) {
        return false;
    }
    return true;
}

bool Connection::should_keep_alive(const http::HttpRequest& request) const noexcept {
    return request.keep_alive() && keep_alive_timeout_seconds_ > 0;
}

}  // namespace server
