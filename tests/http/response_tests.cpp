#include "http/response.hpp"

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

void test_ok_response()
{
auto response = http::HttpResponse::ok("Hello");

expect(
    response.status() == http::StatusCode::OK,
    "Status should be 200 OK"
);

expect(
    response.body() == "Hello",
    "Response body should be preserved"
);

const auto serialized = response.serialize();

expect(
    serialized.find("HTTP/1.1 200 OK") != std::string::npos,
    "Serialized response should contain status line"
);

expect(
    serialized.find("Content-Length: 5") != std::string::npos,
    "Serialized response should contain content length"
);

expect(
    serialized.find("Hello") != std::string::npos,
    "Serialized response should contain body"
);


}

void test_content_type()
{
auto response = http::HttpResponse::ok(
R"({"status":"ok"})"
);

response.set_content_type("application/json");

const auto serialized = response.serialize();

expect(
    serialized.find(
        "Content-Type: application/json"
    ) != std::string::npos,
    "Content-Type should be serialized"
);


}

void test_not_found()
{
auto response = http::HttpResponse::not_found();

expect(
    response.status() == http::StatusCode::NotFound,
    "Status should be 404"
);

const auto serialized = response.serialize();

expect(
    serialized.find("HTTP/1.1 404 Not Found") != std::string::npos,
    "Serialized response should contain 404"
);


}

void test_keep_alive()
{
auto response = http::HttpResponse::ok("Hello");

response.set_keep_alive(true);

const auto serialized = response.serialize();

expect(
    serialized.find("Connection: keep-alive") != std::string::npos,
    "Keep-alive header should be present"
);

response.set_keep_alive(false);

const auto closed = response.serialize();

expect(
    closed.find("Connection: close") != std::string::npos,
    "Connection close header should be present"
);


}

} // namespace

int main()
{
try {
test_ok_response();
test_content_type();
test_not_found();
test_keep_alive();

    std::cout << "response_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "response_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}