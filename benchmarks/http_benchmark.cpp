#include "http/parser.hpp"
#include "http/request.hpp"
#include "http/response.hpp"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using Clock = std::chrono::steady_clock;

struct BenchmarkResult {
std::string name;
std::size_t iterations{};
std::chrono::nanoseconds elapsed{};
};

template <typename Function>
BenchmarkResult benchmark(
std::string name,
std::size_t iterations,
Function&& function
) {
// Warm-up.
function();

const auto start = Clock::now();

for (std::size_t i = 0; i < iterations; ++i) {
    function();
}

const auto end = Clock::now();

return {
    std::move(name),
    iterations,
    std::chrono::duration_cast<std::chrono::nanoseconds>(
        end - start
    )
};


}

void print_result(const BenchmarkResult& result)
{
const double total_ms =
static_cast<double>(result.elapsed.count()) / 1'000'000.0;

const double ns_per_operation =
    static_cast<double>(result.elapsed.count()) /
    static_cast<double>(result.iterations);

const double operations_per_second =
    static_cast<double>(result.iterations) /
    (static_cast<double>(result.elapsed.count()) / 1'000'000'000.0);

std::cout
    << std::left
    << std::setw(28)
    << result.name
    << std::right
    << std::setw(14)
    << result.iterations
    << std::setw(16)
    << std::fixed
    << std::setprecision(2)
    << total_ms
    << std::setw(16)
    << std::setprecision(2)
    << ns_per_operation
    << std::setw(18)
    << std::setprecision(2)
    << operations_per_second
    << '\n';


}

void benchmark_http_parser()
{
constexpr std::size_t iterations = 1'000'000;

constexpr std::string_view request =
    "GET /api/users?id=42 HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Accept: application/json\r\n"
    "Connection: keep-alive\r\n"
    "User-Agent: benchmark-client\r\n"
    "\r\n";

http::HttpParser parser;

const auto result = benchmark(
    "HTTP request parsing",
    iterations,
    [&]() {
        const auto parsed = parser.parse(request);

        if (!parsed.success()) {
            throw std::runtime_error(
                "HTTP parser benchmark produced an invalid request"
            );
        }

        // Prevent an optimizer from treating the result as irrelevant.
        if (parsed.request->target().empty()) {
            throw std::runtime_error(
                "Unexpected empty request target"
            );
        }
    }
);

print_result(result);


}

void benchmark_http_response()
{
constexpr std::size_t iterations = 1'000'000;

const auto result = benchmark(
    "HTTP response serialization",
    iterations,
    []() {
        auto response = http::HttpResponse::ok(
            "Hello World!"
        );

        response.set_content_type("text/plain");
        response.set_keep_alive(true);

        const auto serialized = response.serialize();

        if (serialized.empty()) {
            throw std::runtime_error(
                "HTTP response serialization produced empty output"
            );
        }
    }
);

print_result(result);


}

void benchmark_header_lookup()
{
constexpr std::size_t iterations = 1'000'000;

http::Headers headers;

headers.set("Host", "localhost");
headers.set("Content-Type", "application/json");
headers.set("Content-Length", "128");
headers.set("Connection", "keep-alive");
headers.set("User-Agent", "benchmark-client");

const auto result = benchmark(
    "HTTP header lookup",
    iterations,
    [&]() {
        const auto* value = headers.get("Content-Type");

        if (value == nullptr || value->empty()) {
            throw std::runtime_error(
                "Expected Content-Type header"
            );
        }
    }
);

print_result(result);


}

} // namespace

int main()
{
constexpr int separator_width = 98;

std::cout << '\n';
std::cout << std::string(separator_width, '=') << '\n';
std::cout << "C++ HTTPS Server - HTTP Benchmarks\n";
std::cout << std::string(separator_width, '=') << '\n';

std::cout
    << std::left
    << std::setw(28)
    << "Benchmark"
    << std::right
    << std::setw(14)
    << "Iterations"
    << std::setw(16)
    << "Total (ms)"
    << std::setw(16)
    << "ns/op"
    << std::setw(18)
    << "ops/sec"
    << '\n';

std::cout << std::string(separator_width, '-') << '\n';

try {
    benchmark_http_parser();
    benchmark_http_response();
    benchmark_header_lookup();
}
catch (const std::exception& exception) {
    std::cerr << '\n'
              << "Benchmark failed: "
              << exception.what()
              << '\n';

    return 1;
}

std::cout << std::string(separator_width, '=') << '\n';

return 0;


}