#pragma once

#include "http/http_types.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace http {

class Headers {
public:
using Storage = std::unordered_map<std::string, std::string>;

void set(std::string name, std::string value);

[[nodiscard]]
bool contains(std::string_view name) const;

[[nodiscard]]
const std::string* get(std::string_view name) const;

[[nodiscard]]
std::string value_or(
    std::string_view name,
    std::string_view fallback = {}
) const;

[[nodiscard]]
const Storage& all() const noexcept;

void clear() noexcept;

[[nodiscard]]
std::size_t size() const noexcept;


private:
Storage headers_;
};

class HttpRequest {
public:
HttpRequest() = default;

HttpRequest(
    HttpMethod method,
    std::string target,
    HttpVersion version
);

[[nodiscard]]
HttpMethod method() const noexcept;

[[nodiscard]]
std::string_view target() const noexcept;

[[nodiscard]]
HttpVersion version() const noexcept;

[[nodiscard]]
std::string_view path() const noexcept;

[[nodiscard]]
std::string_view query() const noexcept;

[[nodiscard]]
const Headers& headers() const noexcept;

[[nodiscard]]
Headers& headers() noexcept;

[[nodiscard]]
std::string_view body() const noexcept;

void set_body(std::string body);

void set_target(std::string target);

[[nodiscard]]
bool keep_alive() const noexcept;


private:
HttpMethod method_{HttpMethod::UNKNOWN};
std::string target_;
HttpVersion version_{HttpVersion::UNKNOWN};

Headers headers_;
std::string body_;


};

} // namespace http