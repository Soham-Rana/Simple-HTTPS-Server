#include "http/request.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

namespace http {

namespace {

std::string to_lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string_view trim(std::string_view value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string_view::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

}  // namespace

void Headers::set(std::string name, std::string value) {
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    headers_[std::move(name)] = std::move(value);
}

bool Headers::contains(std::string_view name) const {
    const auto lowered = to_lower(std::string(name));
    return headers_.find(lowered) != headers_.end();
}

const std::string* Headers::get(std::string_view name) const {
    const auto lowered = to_lower(std::string(name));
    const auto it = headers_.find(lowered);
    if (it == headers_.end()) {
        return nullptr;
    }
    return &it->second;
}

std::string Headers::value_or(std::string_view name, std::string_view fallback) const {
    if (const auto* value = get(name)) {
        return *value;
    }
    return std::string(fallback);
}

const Headers::Storage& Headers::all() const noexcept { return headers_; }

void Headers::clear() noexcept { headers_.clear(); }

std::size_t Headers::size() const noexcept { return headers_.size(); }

HttpRequest::HttpRequest(HttpMethod method, std::string target, HttpVersion version)
    : method_(method), target_(std::move(target)), version_(version) {}

HttpMethod HttpRequest::method() const noexcept { return method_; }

std::string_view HttpRequest::target() const noexcept { return target_; }

HttpVersion HttpRequest::version() const noexcept { return version_; }

std::string_view HttpRequest::path() const noexcept {
    const auto pos = target_.find('?');
    if (pos == std::string::npos) {
        return target_;
    }
    return std::string_view(target_).substr(0, pos);
}

std::string_view HttpRequest::query() const noexcept {
    const auto pos = target_.find('?');
    if (pos == std::string::npos) {
        return {};
    }
    return std::string_view(target_).substr(pos + 1);
}

const Headers& HttpRequest::headers() const noexcept { return headers_; }

Headers& HttpRequest::headers() noexcept { return headers_; }

std::string_view HttpRequest::body() const noexcept { return body_; }

void HttpRequest::set_body(std::string body) { body_ = std::move(body); }

void HttpRequest::set_target(std::string target) { target_ = std::move(target); }

bool HttpRequest::keep_alive() const noexcept {
    const auto* connection = headers_.get("Connection");
    if (connection != nullptr) {
        const auto value = trim(std::string_view(*connection));
        return value.empty() || to_lower(std::string(value)) != "close";
    }
    return version_ == HttpVersion::HTTP_1_1;
}

}  // namespace http
