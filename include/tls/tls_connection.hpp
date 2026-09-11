#pragma once

#include "net/socket.hpp"
#include "tls/tls_context.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

#include <openssl/ssl.h>

namespace tls {

/**

TLS session associated with one client connection.

TlsContext contains shared server configuration, while TlsConnection

represents an individual TLS session.
*/
class TlsConnection {
public:
TlsConnection(
net::Socket& socket,
std::shared_ptr<TlsContext> context
);

~TlsConnection();

TlsConnection(const TlsConnection&) = delete;
TlsConnection& operator=(const TlsConnection&) = delete;

TlsConnection(TlsConnection&&) = delete;
TlsConnection& operator=(TlsConnection&&) = delete;

/**

Perform the TLS handshake.
@return true when the handshake succeeds.
*/
[[nodiscard]]
bool handshake();

/**

Read decrypted application data.
@return number of bytes read.
*/
[[nodiscard]]
std::ptrdiff_t read(
std::span<std::byte> buffer
);

/**

Write application data.
The data is encrypted by TLS before being sent over the socket.
@return number of bytes written.
*/
[[nodiscard]]
std::ptrdiff_t write(
std::span<const std::byte> data
);

/**

Write a string over the TLS connection.
*/
[[nodiscard]]
std::ptrdiff_t write(
std::string_view data
);

/**

Perform an orderly TLS shutdown.
*/
void shutdown() noexcept;

/**

Release TLS resources.
The underlying network socket is not owned by TlsConnection and
therefore is not closed here.
*/
void close() noexcept;

[[nodiscard]]
bool handshake_complete() const noexcept;

[[nodiscard]]
bool open() const noexcept;

/**
 * Returns the underlying OpenSSL SSL object.
 * The returned pointer is owned by this object.
 */
[[nodiscard]]
SSL* native_handle() const noexcept;

private:
net::Socket& socket_;
std::shared_ptr<TlsContext> context_;

SSL* ssl_{nullptr};

bool handshake_complete_{false};
bool open_{true};


};

} // namespace tls