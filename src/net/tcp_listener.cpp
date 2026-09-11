#include "net/tcp_listener.hpp"

#include <stdexcept>
#include <utility>

namespace net {

TcpListener::TcpListener(std::string host, std::uint16_t port, int backlog)
    : host_(std::move(host)), port_(port), backlog_(backlog) {
    address_ = Address(host_, port_);
}

void TcpListener::open() {
    socket_ = Socket::tcp_ipv4();
    socket_.set_reuse_address(true);
    socket_.bind(host_, port_);
    socket_.listen(backlog_);
    running_ = true;
    address_ = Address(host_, port_);
}

void TcpListener::start() {
    if (!socket_.valid()) {
        open();
    }
    running_ = true;
}

Socket TcpListener::accept() {
    if (!running_ || !socket_.valid()) {
        throw std::runtime_error("Listener is not running");
    }
    return socket_.accept();
}

void TcpListener::stop() noexcept {
    running_ = false;
    socket_.close();
}

bool TcpListener::running() const noexcept { return running_; }

const Address& TcpListener::address() const noexcept { return address_; }

int TcpListener::backlog() const noexcept { return backlog_; }

}  // namespace net
