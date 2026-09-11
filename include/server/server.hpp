#pragma once

#include "http/router.hpp"
#include "net/tcp_listener.hpp"
#include "server/server_config.hpp"
#include "thread/thread_pool.hpp"
#include "tls/tls_context.hpp"

#include <atomic>
#include <cstddef>
#include <memory>

namespace server {

/**

Main HTTPS server.
Responsible for coordinating:
TCP listener
  ↓

TLS context
  ↓

Client connections
  ↓

HTTP router
  ↓

Worker threads
*/
class Server {
public:
explicit Server(ServerConfig config);
~Server();

Server(const Server&) = delete;
Server& operator=(const Server&) = delete;

Server(Server&&) = delete;
Server& operator=(Server&&) = delete;

/**
 * Start the server.
 *
 * This method blocks until stop() is called or a fatal server error occurs.
 */
void run();

/**
 * Request graceful server shutdown.
 */
void stop() noexcept;

/**
 * Register an HTTP route.
 */
http::Router& router() noexcept;

[[nodiscard]]
const http::Router& router() const noexcept;

[[nodiscard]]
bool running() const noexcept;

[[nodiscard]]
std::size_t active_connections() const noexcept;

[[nodiscard]]
const ServerConfig& config() const noexcept;


private:
/**
* Initialize networking and TLS resources.
*/
void initialize();

/**
 * Accept incoming TCP connections.
 */
void accept_connections();

/**
 * Submit a client connection to the worker pool.
 */
void handle_connection(net::Socket socket);

/**
 * Release server resources during shutdown.
 */
void shutdown() noexcept;

ServerConfig config_;

std::unique_ptr<net::TcpListener> listener_;
std::shared_ptr<tls::TlsContext> tls_context_;
std::unique_ptr<thread::ThreadPool> thread_pool_;

http::Router router_;

std::atomic<bool> running_{false};
std::atomic<bool> stopping_{false};
std::atomic<std::size_t> active_connections_{0};


};

} // namespace server