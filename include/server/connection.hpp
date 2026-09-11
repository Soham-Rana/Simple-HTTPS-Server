#pragma once

#include "http/parser.hpp"
#include "http/request.hpp"
#include "http/response.hpp"
#include "http/router.hpp"
#include "net/socket.hpp"
#include "tls/tls_connection.hpp"

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include<cstdint>
#include<optional>

namespace server {

/**
 * Represents a single client connection.
 * A connection owns the underlying network socket and, when TLS is enabled,
 * its associated TLS session.
 *
 * Responsibilities:
 * - Manage the client connection lifecycle.
 * - Perform TLS communication.
 * - Read HTTP requests.
 * - Write HTTP responses.
 * - Enforce connection-level limits and timeouts.
 */
class Connection {
public:
    Connection(
        net::Socket socket,
        std::shared_ptr<tls::TlsContext> tls_context,
        http::ParseLimits http_limits,
        std::uint32_t request_timeout_seconds,
        std::uint32_t keep_alive_timeout_seconds
    );
~Connection();

Connection(const Connection&) = delete;
Connection& operator=(const Connection&) = delete;

Connection(Connection&&) = delete;
Connection& operator=(Connection&&) = delete;

/**
 * Perform the TLS handshake.
 *
 * Returns false if the handshake fails.
 */
[[nodiscard]]
bool handshake();

/**
 * Process requests on this connection.
 *
 * The supplied callback is invoked for every successfully parsed request.
 */
void run(
    const http::RequestHandler& handler
);

/**
 * Close the connection.
 */
void close() noexcept;

[[nodiscard]]
bool is_open() const noexcept;

[[nodiscard]]
bool is_tls() const noexcept;


private:
    /**
     * Read encrypted/plain bytes from the connection.
     */
    [[nodiscard]]
    std::ptrdiff_t read(
        char* buffer,
        std::size_t size
    );

/**
 * Write encrypted/plain bytes to the connection.
 */
[[nodiscard]]
std::ptrdiff_t write(
    const char* data,
    std::size_t size
);

/**
 * Read and parse one HTTP request.
 */
[[nodiscard]]
std::optional<http::HttpRequest> read_request();

/**
 * Send an HTTP response.
 */
bool send_response(
    const http::HttpResponse& response
);

/**
 * Determine whether the connection should remain open.
 */
[[nodiscard]]
bool should_keep_alive(
    const http::HttpRequest& request
) const noexcept;

net::Socket socket_;

std::shared_ptr<tls::TlsContext> tls_context_;
std::unique_ptr<tls::TlsConnection> tls_connection_;

http::HttpParser parser_;

std::uint32_t request_timeout_seconds_;
std::uint32_t keep_alive_timeout_seconds_;

std::string read_buffer_;

bool tls_enabled_{true};
bool open_{true};
bool handshake_complete_{false};


};

} // namespace server