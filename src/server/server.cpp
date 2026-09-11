#include "server/server.hpp"

#include "server/connection.hpp"

#include <thread>

namespace server {

Server::Server(ServerConfig config) : config_(std::move(config)) {
    initialize();
}

Server::~Server() {
    shutdown();
}

void Server::initialize() {
    listener_ = std::make_unique<net::TcpListener>(config_.host, config_.port, config_.listen_backlog);
    if (config_.tls_enabled) {
        tls_context_ = std::make_shared<tls::TlsContext>(config_.certificate_file, config_.private_key_file);
    }
    thread_pool_ = std::make_unique<thread::ThreadPool>(config_.worker_threads);
}

void Server::run() {
    running_ = true;
    listener_->open();
    while (running_ && !stopping_) {
        try {
            auto socket = listener_->accept();
            handle_connection(std::move(socket));
        } catch (...) {
            if (stopping_) {
                break;
            }
        }
    }
    shutdown();
}

void Server::stop() noexcept {
    stopping_ = true;
    running_ = false;
    if (listener_) {
        listener_->stop();
    }
}

http::Router& Server::router() noexcept { return router_; }

const http::Router& Server::router() const noexcept { return router_; }

bool Server::running() const noexcept { return running_.load(); }

std::size_t Server::active_connections() const noexcept { return active_connections_.load(); }

const ServerConfig& Server::config() const noexcept { return config_; }

void Server::accept_connections() {
    while (running_ && !stopping_) {
        try {
            auto socket = listener_->accept();
            handle_connection(std::move(socket));
        } catch (...) {
            break;
        }
    }
}

void Server::handle_connection(net::Socket socket) {
    ++active_connections_;
    auto connection = std::make_shared<Connection>(
        std::move(socket),
        tls_context_,
        http::ParseLimits{config_.max_request_line_size, config_.max_header_size, config_.max_body_size},
        config_.request_timeout_seconds,
        config_.keep_alive_timeout_seconds);

    if (connection->handshake()) {
        auto task = [this, connection]() mutable {
            connection->run([this](const http::HttpRequest& request) {
                return router_.route(request);
            });
            --active_connections_;
        };
        thread_pool_->submit(task);
    } else {
        --active_connections_;
    }
}

void Server::shutdown() noexcept {
    stopping_ = true;
    running_ = false;
    if (listener_) {
        listener_->stop();
    }
    if (thread_pool_) {
        thread_pool_->shutdown();
    }
}

}  // namespace server
