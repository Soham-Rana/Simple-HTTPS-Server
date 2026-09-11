#include "http/request.hpp"

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

void test_headers()
{
http::Headers headers;

headers.set("Host", "localhost");
headers.set("Content-Type", "application/json");

expect(
    headers.contains("Host"),
    "Host header should exist"
);

expect(
    headers.contains("Content-Type"),
    "Content-Type header should exist"
);

expect(
    headers.value_or("Host") == "localhost",
    "Host value should be localhost"
);

expect(
    headers.value_or("Missing", "fallback") == "fallback",
    "Missing header should return fallback"
);


}

void test_header_replacement()
{
http::Headers headers;

headers.set("Host", "old.example");
headers.set("Host", "new.example");

expect(
    headers.value_or("Host") == "new.example",
    "Setting a header should replace its previous value"
);

expect(
    headers.size() == 1,
    "Replacing a header should not increase header count"
);


}

void test_request_properties()
{
http::HttpRequest request(
http::HttpMethod::POST,
"/api/users?id=42",
http::HttpVersion::HTTP_1_1
);

request.headers().set("Host", "localhost");
request.set_body(R"({"name":"Alice"})");

expect(
    request.method() == http::HttpMethod::POST,
    "Method should be POST"
);

expect(
    request.target() == "/api/users?id=42",
    "Target should be preserved"
);

expect(
    request.path() == "/api/users",
    "Path should be extracted"
);

expect(
    request.query() == "id=42",
    "Query should be extracted"
);

expect(
    request.body() == R"({"name":"Alice"})",
    "Body should be preserved"
);


}

} // namespace

int main()
{
try {
test_headers();
test_header_replacement();
test_request_properties();

    std::cout << "request_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "request_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}