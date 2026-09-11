#include "net/socket.hpp"
#include "tls/tls_connection.hpp"
#include "tls/tls_context.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace {

void expect(
bool condition,
std::string_view message
)
{
if (!condition) {
throw std::runtime_error(
std::string("Assertion failed: ") +
std::string(message)
);
}
}

void test_socket_starts_invalid()
{
net::Socket socket;

expect(
    !socket.valid(),
    "Default socket should be invalid"
);

expect(
    socket.native_handle() == net::Socket::InvalidHandle,
    "Invalid socket should have invalid native handle"
);


}

void test_tls_connection_requires_context()
{
net::Socket socket;

bool threw = false;

try {
    auto context = std::shared_ptr<tls::TlsContext>{};

    tls::TlsConnection connection(
        socket,
        std::move(context)
    );
}
catch (const std::exception&) {
    threw = true;
}

expect(
    threw,
    "TLS connection should reject a null context"
);


}

} // namespace

int main()
{
try {
test_socket_starts_invalid();
test_tls_connection_requires_context();

    std::cout << "tls_connection_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "tls_connection_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}