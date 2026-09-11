#include "server/server_config.hpp"

#include <cstdlib>
#include <iostream>
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

void test_default_configuration()
{
server::ServerConfig config;

expect(
    config.host == "0.0.0.0",
    "Default host should be 0.0.0.0"
);

expect(
    config.port == 8443,
    "Default port should be 8443"
);

expect(
    config.listen_backlog == 128,
    "Default listen backlog should be 128"
);

expect(
    config.worker_threads == 4,
    "Default worker count should be 4"
);

expect(
    config.max_connections == 1024,
    "Default maximum connections should be 1024"
);

expect(
    config.tls_enabled,
    "TLS should be enabled by default"
);


}

void test_valid_configuration()
{
server::ServerConfig config;

config.host = "127.0.0.1";
config.port = 9443;
config.worker_threads = 2;
config.max_connections = 100;

config.certificate_file = "certs/server.crt";
config.private_key_file = "certs/server.key";

config.validate();


}

void test_invalid_port()
{
server::ServerConfig config;
config.port = 0;

bool threw = false;

try {
    config.validate();
}
catch (const std::invalid_argument&) {
    threw = true;
}

expect(
    threw,
    "Port zero should fail validation"
);


}

void test_invalid_worker_count()
{
server::ServerConfig config;
config.worker_threads = 0;

bool threw = false;

try {
    config.validate();
}
catch (const std::invalid_argument&) {
    threw = true;
}

expect(
    threw,
    "Zero worker threads should fail validation"
);


}

void test_invalid_connection_limit()
{
server::ServerConfig config;
config.max_connections = 0;

bool threw = false;

try {
    config.validate();
}
catch (const std::invalid_argument&) {
    threw = true;
}

expect(
    threw,
    "Zero maximum connections should fail validation"
);


}

void test_invalid_tls_configuration()
{
server::ServerConfig config;

config.tls_enabled = true;
config.certificate_file.clear();

bool threw = false;

try {
    config.validate();
}
catch (const std::invalid_argument&) {
    threw = true;
}

expect(
    threw,
    "TLS without certificate path should fail validation"
);


}

} // namespace

int main()
{
try {
test_default_configuration();
test_valid_configuration();
test_invalid_port();
test_invalid_worker_count();
test_invalid_connection_limit();
test_invalid_tls_configuration();

    std::cout << "server_config_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "server_config_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}