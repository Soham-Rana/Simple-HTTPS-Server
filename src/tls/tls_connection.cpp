#include "tls/tls_connection.hpp"

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <stdexcept>

namespace tls {

TlsConnection::TlsConnection(net::Socket& socket, std::shared_ptr<TlsContext> context)
    : socket_(socket), context_(std::move(context)) {
    if (!context_) {
        throw std::invalid_argument("TLS context must not be null");
    }
    ssl_ = SSL_new(context_->native_handle());
    if (ssl_ == nullptr) {
        throw std::runtime_error("Failed to create OpenSSL SSL object");
    }
    if (SSL_set_fd(ssl_, socket_.native_handle()) != 1) {
        SSL_free(ssl_);
        ssl_ = nullptr;
        throw std::runtime_error("Failed to attach socket to TLS connection");
    }
}

TlsConnection::~TlsConnection() { close(); }

bool TlsConnection::handshake() {
    if (ssl_ == nullptr) {
        return false;
    }
    const int status = SSL_accept(ssl_);
    handshake_complete_ = status == 1;
    if (!handshake_complete_) {
        ERR_print_errors_fp(stderr);
    }
    return handshake_complete_;
}

std::ptrdiff_t TlsConnection::read(std::span<std::byte> buffer) {
    if (!open_ || ssl_ == nullptr || buffer.empty()) {
        return 0;
    }
    const int bytes = SSL_read(ssl_, buffer.data(), static_cast<int>(buffer.size()));
    if (bytes <= 0) {
        return -1;
    }
    return static_cast<std::ptrdiff_t>(bytes);
}

std::ptrdiff_t TlsConnection::write(std::span<const std::byte> data) {
    if (!open_ || ssl_ == nullptr || data.empty()) {
        return 0;
    }
    const int bytes = SSL_write(ssl_, data.data(), static_cast<int>(data.size()));
    if (bytes <= 0) {
        return -1;
    }
    return static_cast<std::ptrdiff_t>(bytes);
}

std::ptrdiff_t TlsConnection::write(std::string_view data) {
    if (data.empty()) {
        return 0;
    }
    return write(std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size()));
}

void TlsConnection::shutdown() noexcept {
    if (ssl_ != nullptr) {
        SSL_shutdown(ssl_);
    }
}

void TlsConnection::close() noexcept {
    open_ = false;
    if (ssl_ != nullptr) {
        SSL_free(ssl_);
        ssl_ = nullptr;
    }
}

bool TlsConnection::handshake_complete() const noexcept { return handshake_complete_; }

bool TlsConnection::open() const noexcept { return open_; }

SSL* TlsConnection::native_handle() const noexcept { return ssl_; }

}  // namespace tls
