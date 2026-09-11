#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace net {

/**

RAII wrapper around a native operating-system socket.

Socket ownership is exclusive. The underlying descriptor is closed

automatically when the Socket object is destroyed.
*/
class Socket {
public:
using NativeHandle = int;

static constexpr NativeHandle InvalidHandle = -1;

Socket() noexcept = default;

explicit Socket(NativeHandle handle) noexcept;

~Socket();

Socket(const Socket&) = delete;
Socket& operator=(const Socket&) = delete;

Socket(Socket&& other) noexcept;

Socket& operator=(Socket&& other) noexcept;

/**

Create a TCP socket.
*/
static Socket tcp_ipv4();

/**

Create an IPv6 TCP socket.
*/
static Socket tcp_ipv6();

/**

Bind the socket to a local address.
*/
void bind(
std::string_view host,
std::uint16_t port
);

/**

Mark the socket as a passive listening socket.
*/
void listen(int backlog = 128);

/**

Accept an incoming connection.
*/
[[nodiscard]]
Socket accept();

/**

Receive bytes from the socket.
Returns the number of bytes received.
A return value of zero indicates an orderly peer shutdown.
*/
[[nodiscard]]
std::ptrdiff_t receive(
std::span<std::byte> buffer
);

/**

Send bytes to the socket.
Returns the number of bytes successfully sent.
*/
[[nodiscard]]
std::ptrdiff_t send(
std::span<const std::byte> data
);

/**

Send a string to the socket.
*/
[[nodiscard]]
std::ptrdiff_t send(
std::string_view data
);

/**

Enable or disable address reuse.
*/
void set_reuse_address(bool enabled = true);

/**

Enable or disable TCP_NODELAY.
*/
void set_nodelay(bool enabled = true);

/**

Configure blocking mode.
*/
void set_blocking(bool enabled);

/**

Configure receive timeout in milliseconds.
*/
void set_receive_timeout(std::uint32_t milliseconds);

/**

Configure send timeout in milliseconds.
*/
void set_send_timeout(std::uint32_t milliseconds);

/**

Shut down one or both directions of the connection.
*/
void shutdown(int how = 2) noexcept;

/**

Close the socket.
*/
void close() noexcept;

[[nodiscard]]
NativeHandle native_handle() const noexcept;

[[nodiscard]]
bool valid() const noexcept;

/**

Release ownership of the native socket descriptor.
*/
[[nodiscard]]
NativeHandle release() noexcept;

private:
NativeHandle handle_{InvalidHandle};
};

} // namespace net