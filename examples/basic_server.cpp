#include "http/request.hpp"
#include "http/response.hpp"
#include "server/server.hpp"
#include "server/server_config.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>


namespace {

http::HttpResponse health_handler(const http::HttpRequest&)
{
auto response = http::HttpResponse::ok(
R"({"status":"ok"})"
);

response.set_content_type("application/json");

return response;


}

http::HttpResponse root_handler(const http::HttpRequest&)
{
auto response = http::HttpResponse::ok(
"C++ HTTPS Server\n"
"Status: running\n"
"Protocol: HTTPS / HTTP/1.1\n"
);

response.set_content_type("text/plain");

return response;


}

http::HttpResponse hello_handler(const http::HttpRequest& request)
{
const std::string name = & {
const auto query = request.query();

    constexpr std::string_view prefix = "name=";

    if (query.starts_with(prefix)) {
        return std::string(query.substr(prefix.size()));
    }

    return std::string("World");
}();

auto response = http::HttpResponse::ok(
    "Hello, " + name + "!\n"
);

response.set_content_type("text/plain");

return response;


}

http::HttpResponse not_found_handler(const http::HttpRequest&)
{
auto response = http::HttpResponse::not_found(
"The requested resource was not found.\n"
);

response.set_content_type("text/plain");

return response;


}

} // namespace

int main()
{
try {
server::ServerConfig config;

    config.host = "0.0.0.0";
    config.port = 8443;

    config.worker_threads = 4;
    config.max_connections = 1024;

    config.certificate_file = "certs/server.crt";
    config.private_key_file = "certs/server.key";

    config.validate();

    server::Server server(config);

    // ----------------------------------------------------------------------
    // Routes
    // ----------------------------------------------------------------------

    server.router().get(
        "/",
        root_handler
    );

    server.router().get(
        "/health",
        health_handler
    );

    server.router().get(
        "/hello",
        hello_handler
    );

    // ----------------------------------------------------------------------
    // Start server
    // ----------------------------------------------------------------------

    std::cout
        << "Starting C++ HTTPS server...\n"
        << "Listening on https://"
        << config.host
        << ":"
        << config.port
        << '\n';

    server.run();
}
catch (const std::exception& exception) {
    std::cerr
        << "Fatal error: "
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}

return EXIT_SUCCESS;


}