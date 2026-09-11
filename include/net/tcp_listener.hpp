#pragma once

#include "net/address.hpp"
#include "net/socket.hpp"

#include <cstdint>
#include <string>

namespace net {

/**

TCP server listener.

Responsible only for creating, configuring, binding, and accepting

TCP connections. Higher layers such as TLS and HTTP are handled

outside this class.
*/
class TcpListener {
public:
TcpListener() = default;

TcpListener(
std::string host,
std::uint16_t port,
int backlog = 128
);

~TcpListener() = default;

TcpListener(const TcpListener&) = delete;
TcpListener& operator=(const TcpListener&) = delete;

TcpListener(TcpListener&&) noexcept = default;
TcpListener& operator=(TcpListener&&) noexcept = default;

/**

Create and configure the listening socket.
*/
void open();

/**

Bind and begin listening for incoming connections.
*/
void start();

/**

Accept the next incoming TCP connection.
*/
[[nodiscard]]
Socket accept();

/**

Stop accepting new connections and release the socket.
*/
void stop() noexcept;

[[nodiscard]]
bool running() const noexcept;

[[nodiscard]]
const Address& address() const noexcept;

[[nodiscard]]
int backlog() const noexcept;

private:
std::string host_{"0.0.0.0"};
std::uint16_t port_{8080};
int backlog_{128};

Address address_;
Socket socket_;
bool running_{false};


};

} // namespace net