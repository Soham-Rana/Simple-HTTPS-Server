#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace server {

struct ServerConfig {
// --------------------------------------------------------------------------
// Network
// --------------------------------------------------------------------------

std::string host{"0.0.0.0"};
std::uint16_t port{8443};

int listen_backlog{128};

// --------------------------------------------------------------------------
// Concurrency
// --------------------------------------------------------------------------

std::size_t worker_threads{4};
std::size_t max_connections{1024};

// --------------------------------------------------------------------------
// HTTP limits
// --------------------------------------------------------------------------

std::size_t max_request_line_size{8192};
std::size_t max_header_size{16384};
std::size_t max_body_size{10 * 1024 * 1024};

// --------------------------------------------------------------------------
// Timeouts
// --------------------------------------------------------------------------

std::uint32_t request_timeout_seconds{30};
std::uint32_t keep_alive_timeout_seconds{5};
std::uint32_t tls_handshake_timeout_seconds{10};

// --------------------------------------------------------------------------
// TLS
// --------------------------------------------------------------------------

bool tls_enabled{true};

std::string certificate_file{"certs/server.crt"};
std::string private_key_file{"certs/server.key"};

// --------------------------------------------------------------------------
// Logging
// --------------------------------------------------------------------------

std::string log_level{"info"};
std::string log_file{};

// --------------------------------------------------------------------------
// Runtime
// --------------------------------------------------------------------------

bool reuse_address{true};
bool tcp_no_delay{true};

/**
 * Validate configuration values.
 *
 * Throws std::invalid_argument when configuration is invalid.
 */
void validate() const;


};

} // namespace server