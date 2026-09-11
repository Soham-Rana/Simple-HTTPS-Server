#include "http/request.hpp"
#include "http/response.hpp"
#include "http/router.hpp"

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

void test_get_route()
{
http::Router router;

router.get(
    "/hello",
    [](const http::HttpRequest&) {
        return http::HttpResponse::ok("Hello");
    }
);

http::HttpRequest request(
    http::HttpMethod::GET,
    "/hello",
    http::HttpVersion::HTTP_1_1
);

const auto response = router.route(request);

expect(
    response.status() == http::StatusCode::OK,
    "GET route should return 200"
);

expect(
    response.body() == "Hello",
    "GET route should return expected body"
);


}

void test_post_route()
{
http::Router router;

router.post(
    "/users",
    [](const http::HttpRequest& request) {
        return http::HttpResponse::created(
            std::string(request.body())
        );
    }
);

http::HttpRequest request(
    http::HttpMethod::POST,
    "/users",
    http::HttpVersion::HTTP_1_1
);

request.set_body(R"({"name":"Alice"})");

const auto response = router.route(request);

expect(
    response.status() == http::StatusCode::Created,
    "POST route should return 201"
);

expect(
    response.body() == R"({"name":"Alice"})",
    "POST handler should receive request body"
);


}

void test_not_found()
{
http::Router router;

router.get(
    "/hello",
    [](const http::HttpRequest&) {
        return http::HttpResponse::ok("Hello");
    }
);

http::HttpRequest request(
    http::HttpMethod::GET,
    "/missing",
    http::HttpVersion::HTTP_1_1
);

const auto response = router.route(request);

expect(
    response.status() == http::StatusCode::NotFound,
    "Unknown route should return 404"
);


}

void test_method_not_allowed()
{
http::Router router;

router.get(
    "/users",
    [](const http::HttpRequest&) {
        return http::HttpResponse::ok("Users");
    }
);

http::HttpRequest request(
    http::HttpMethod::POST,
    "/users",
    http::HttpVersion::HTTP_1_1
);

const auto response = router.route(request);

expect(
    response.status() == http::StatusCode::MethodNotAllowed,
    "Wrong method should return 405"
);


}

void test_contains()
{
http::Router router;

router.get(
    "/health",
    [](const http::HttpRequest&) {
        return http::HttpResponse::ok("ok");
    }
);

expect(
    router.contains(http::HttpMethod::GET, "/health"),
    "Registered route should be found"
);

expect(
    !router.contains(http::HttpMethod::POST, "/health"),
    "Unregistered method should not be found"
);

expect(
    !router.contains(http::HttpMethod::GET, "/missing"),
    "Unregistered path should not be found"
);


}

} // namespace

int main()
{
try {
test_get_route();
test_post_route();
test_not_found();
test_method_not_allowed();
test_contains();

    std::cout << "router_tests: PASS\n";
    return EXIT_SUCCESS;
}
catch (const std::exception& exception) {
    std::cerr
        << "router_tests: FAIL\n"
        << exception.what()
        << '\n';

    return EXIT_FAILURE;
}


}