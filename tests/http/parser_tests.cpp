#include "http/parser.hpp"
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

void test_get_request()
{
constexpr std::string_view raw_request =
"GET /hello HTTP/1.1\r\n"
"Host: localhost\r\n"
"User-Agent: test-client\r\n"
"Accept: /\r\n"
"\r\n";

http::HttpParser parser;
const auto result = parser.parse(raw_request);

expect(result.success(), "GET request should parse");
expect(result.request.has_value(), "Request should exist");

const auto& request = *result.request;

expect(
    request.method() == http::HttpMethod::GET,
    "Method should be GET"
);

expect(
    request.target() == "/hello",
    "Target should be /hello"
);

expect(
    request.path() == "/hello",
    "Path should be /hello"
);

expect(
    request.version() == http::HttpVersion::HTTP_1_1,
    "Version should be HTTP/1.1"
);

expect(
    request.headers().contains("Host"),
    "Host header should exist"
);

expect(
    request.headers().value_or("Host") == "localhost",
    "Host header should contain localhost"
);


}

void test_query_string()
{
constexpr std::string_view raw_request =
"GET /search?q=cpp&limit=10 HTTP/1.1\r\n"
"Host: localhost\r\n"
"\r\n";

http::HttpParser parser;
const auto result = parser.parse(raw_request);

expect(result.success(), "Query request should parse");

const auto& request = *result.request;

expect(
    request.path() == "/search",
    "Path should exclude query string"
);

expect(
    request.query() == "q=cpp&limit=10",
    "Query string should be parsed"
);


}

void test_post_request_with_body()
{
constexpr std::string_view raw_request =
"POST /api/users HTTP/1.1\r\n"
"Host: localhost\r\n"
"Content-Type: application/json\r\n"
"Content-Length: 15\r\n"
"\r\n"
R"({"name":"John"})";

http::HttpParser parser;
const auto result = parser.parse(raw_request);

expect(result.success(), "POST request should parse");

const auto& request = *result.request;

expect(
    request.method() == http::HttpMethod::POST,
    "Method should be POST"
);

expect(
    request.body() == R"({"name":"John"})",
    "Body should be parsed"
);

expect(
    request.headers().value_or("Content-Type") ==
        "application/json",
    "Content-Type should be parsed"
);


}

void test_http_10()
{
constexpr std::string_view raw_request =
"GET / HTTP/1.0\r\n"
"Host: localhost\r\n"
"\r\n";

http::HttpParser parser;
const auto result = parser.parse(raw_request);

expect(result.success(), "HTTP/1.0 request should parse");

expect(
    result.request->version() == http::HttpVersion::HTTP_1_0,
    "Version should be HTTP/1.0"
);


}

void test_invalid_request_line()
{
constexpr std::string_view raw_request =
"INVALID REQUEST\r\n"
"Host: localhost\r\n"
"\r\n";

http::HttpParser parser;
const auto result = parser.parse(raw_request);

expect(
    !result.success(),
    "Invalid request should fail"
);

expect(
    result.error != http::ParseError::None,
    "Parser should report an error"
);


}

void test_incomplete_request()
{
constexpr std::string_view raw_request =
"GET / HTTP/1.1\r\n"
"Host: localhost\r\n";

http::HttpParser parser;
const auto result = parser.parse(raw_request);

expect(
    result.error == http::ParseError::IncompleteRequest,
    "Incomplete request should be reported"
);


}

} // namespace

int main()
{
try {
test_get_request();
test_query_string();
test_post_request_with_body();
test_http_10();
test_invalid_request_line();
test_incomplete_request();

    std::cout << "parser_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "parser_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}