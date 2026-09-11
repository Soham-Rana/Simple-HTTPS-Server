#include "server/server_config.hpp"

#include <stdexcept>
#include <string>

namespace server {

void ServerConfig::validate() const {
    if (port == 0) {
        throw std::invalid_argument("Port must be greater than zero");
    }
    if (worker_threads == 0) {
        throw std::invalid_argument("Worker thread count must be greater than zero");
    }
    if (max_connections == 0) {
        throw std::invalid_argument("Maximum connection count must be greater than zero");
    }
    if (tls_enabled) {
        if (certificate_file.empty()) {
            throw std::invalid_argument("TLS certificate file cannot be empty");
        }
        if (private_key_file.empty()) {
            throw std::invalid_argument("TLS private key file cannot be empty");
        }
    }
}

}  // namespace server
