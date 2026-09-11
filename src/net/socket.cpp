#include "net/socket.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <cerrno>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace net {
namespace {

#ifdef _WIN32
bool ensure_winsock_initialized() {
    static std::once_flag flag;
    static bool initialized = false;

    std::call_once(flag, []() {
        WSADATA data{};
        initialized = WSAStartup(MAKEWORD(2, 2), &data) == 0;
    });

    return initialized;
}

void throw_last_error(const std::string& action) {
    throw std::runtime_error(action + ": WSA error " + std::to_string(WSAGetLastError()));
}

#else

void throw_last_error(const std::string& action) {
    throw std::runtime_error(action + ": " + std::strerror(errno));
}

#endif

}  // namespace

Socket::Socket(NativeHandle handle) noexcept : handle_(handle) {}

Socket::~Socket() { close(); }

Socket::Socket(Socket&& other) noexcept : handle_(other.handle_) { other.handle_ = InvalidHandle; }

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = InvalidHandle;
    }
    return *this;
}

Socket Socket::tcp_ipv4() {
#ifdef _WIN32
    if (!ensure_winsock_initialized()) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
    const auto handle = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (handle == InvalidHandle) {
        throw_last_error("socket");
    }
    return Socket(handle);
}

Socket Socket::tcp_ipv6() {
#ifdef _WIN32
    if (!ensure_winsock_initialized()) {
        throw std::runtime_error("WSAStartup failed");
    }
#endif
    const auto handle = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (handle == InvalidHandle) {
        throw_last_error("socket");
    }
    return Socket(handle);
}

void Socket::bind(std::string_view host, std::uint16_t port) {
    if (!valid()) {
        throw std::runtime_error("Socket is not valid");
    }

    sockaddr_in address{};
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (host.empty() || host == "0.0.0.0") {
        address.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        if (::inet_pton(AF_INET, std::string(host).c_str(), &address.sin_addr) != 1) {
            throw std::runtime_error("Invalid IPv4 host");
        }
    }

    if (::bind(handle_, reinterpret_cast<const sockaddr*>(&address), sizeof(address)) != 0) {
        throw_last_error("bind");
    }
}

void Socket::listen(int backlog) {
    if (!valid()) {
        throw std::runtime_error("Socket is not valid");
    }
    if (::listen(handle_, backlog) != 0) {
        throw_last_error("listen");
    }
}

Socket Socket::accept() {
    if (!valid()) {
        throw std::runtime_error("Socket is not valid");
    }

    sockaddr client_address{};
    socklen_t client_length = sizeof(client_address);
    const auto accepted = ::accept(handle_, &client_address, &client_length);
    if (accepted == InvalidHandle) {
        throw_last_error("accept");
    }
    return Socket(accepted);
}

std::ptrdiff_t Socket::receive(std::span<std::byte> buffer) {
    if (!valid()) {
        return -1;
    }
    if (buffer.empty()) {
        return 0;
    }
    const auto* raw = reinterpret_cast<const char*>(buffer.data());
    const auto received = ::recv(handle_, const_cast<char*>(raw), static_cast<int>(buffer.size()), 0);
    if (received < 0) {
#ifndef _WIN32
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
#else
        if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR) {
            return 0;
        }
#endif
        return -1;
    }
    return static_cast<std::ptrdiff_t>(received);
}

std::ptrdiff_t Socket::send(std::span<const std::byte> data) {
    if (!valid()) {
        return -1;
    }
    if (data.empty()) {
        return 0;
    }
    const auto* raw = reinterpret_cast<const char*>(data.data());
    const auto sent = ::send(handle_, raw, static_cast<int>(data.size()), 0);
    if (sent < 0) {
#ifndef _WIN32
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
#else
        if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINTR) {
            return 0;
        }
#endif
        return -1;
    }
    return static_cast<std::ptrdiff_t>(sent);
}

std::ptrdiff_t Socket::send(std::string_view data) {
    if (data.empty()) {
        return 0;
    }
    return send(std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), data.size()));
}

void Socket::set_reuse_address(bool enabled) {
    if (!valid()) {
        return;
    }
    const int value = enabled ? 1 : 0;
    if (::setsockopt(handle_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&value), sizeof(value)) != 0) {
        throw_last_error("setsockopt(SO_REUSEADDR)");
    }
}

void Socket::set_nodelay(bool enabled) {
    if (!valid()) {
        return;
    }
    const int value = enabled ? 1 : 0;
    if (::setsockopt(handle_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&value), sizeof(value)) != 0) {
        throw_last_error("setsockopt(TCP_NODELAY)");
    }
}

void Socket::set_blocking(bool enabled) {
    if (!valid()) {
        return;
    }
#ifdef _WIN32
    u_long mode = enabled ? 0 : 1;
    if (::ioctlsocket(handle_, FIONBIO, &mode) != 0) {
        throw_last_error("ioctlsocket(FIONBIO)");
    }
#else
    const auto flags = ::fcntl(handle_, F_GETFL, 0);
    if (flags == -1) {
        throw_last_error("fcntl(F_GETFL)");
    }
    if (enabled) {
        ::fcntl(handle_, F_SETFL, flags & ~O_NONBLOCK);
    } else {
        ::fcntl(handle_, F_SETFL, flags | O_NONBLOCK);
    }
#endif
}

void Socket::set_receive_timeout(std::uint32_t milliseconds) {
    if (!valid()) {
        return;
    }
    timeval timeout{};
    timeout.tv_sec = static_cast<time_t>(milliseconds / 1000);
    timeout.tv_usec = static_cast<decltype(timeout.tv_usec)>((milliseconds % 1000) * 1000);
    if (::setsockopt(handle_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) != 0) {
        throw_last_error("setsockopt(SO_RCVTIMEO)");
    }
}

void Socket::set_send_timeout(std::uint32_t milliseconds) {
    if (!valid()) {
        return;
    }
    timeval timeout{};
    timeout.tv_sec = static_cast<time_t>(milliseconds / 1000);
    timeout.tv_usec = static_cast<decltype(timeout.tv_usec)>((milliseconds % 1000) * 1000);
    if (::setsockopt(handle_, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout)) != 0) {
        throw_last_error("setsockopt(SO_SNDTIMEO)");
    }
}

void Socket::shutdown(int how) noexcept {
    if (valid()) {
        ::shutdown(handle_, how);
    }
}

void Socket::close() noexcept {
    if (valid()) {
#ifdef _WIN32
        ::closesocket(handle_);
#else
        ::close(handle_);
#endif
        handle_ = InvalidHandle;
    }
}

Socket::NativeHandle Socket::native_handle() const noexcept { return handle_; }

bool Socket::valid() const noexcept { return handle_ != InvalidHandle; }

Socket::NativeHandle Socket::release() noexcept {
    const auto handle = handle_;
    handle_ = InvalidHandle;
    return handle;
}

}  // namespace net
