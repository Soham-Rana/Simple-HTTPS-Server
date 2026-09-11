#include "http/request.hpp"
#include "http/response.hpp"
#include "server/server.hpp"
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

server::ServerConfig test_config()
{
server::ServerConfig config;

config.host = "127.0.0.1";
config.port = 0;

config.worker_threads = 2;
config.max_connections = 16;

config.tls_enabled = false;

return config;


}

void test_server_initial_state()
{
server::Server server(test_config());

expect(
    !server.running(),
    "Server should not be running immediately after construction"
);

expect(
    server.active_connections() == 0,
    "Server should have zero active connections"
);


}

void test_router_access()
{
server::Server server(test_config());

server.router().get(
    "/health",
    [](const http::HttpRequest&) {
        return http::HttpResponse::ok("ok");
    }
);

expect(
    server.router().contains(
        http::HttpMethod::GET,
        "/health"
    ),
    "Server router should contain registered route"
);


}

void test_const_router_access()
{
server::Server server(test_config());

server.router().get(
    "/",
    [](const http::HttpRequest&) {
        return http::HttpResponse::ok("hello");
    }
);

const auto& const_server = server;

expect(
    const_server.router().contains(
        http::HttpMethod::GET,
        "/"
    ),
    "Const router access should work"
);


}

void test_stop_before_run()
{
server::Server server(test_config());

server.stop();

expect(
    !server.running(),
    "Server should remain stopped"
);


}

} // namespace

int main()
{
try {
test_server_initial_state();
test_router_access();
test_const_router_access();
test_stop_before_run();

    std::cout << "server_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "server_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}