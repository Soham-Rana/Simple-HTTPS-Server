#include "http/parser.hpp"

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <utility>

namespace http {
namespace {

std::string trim(std::string_view value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string_view::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(begin, end - begin + 1));
}

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::optional<std::string> parse_header_value(std::string_view header_line) {
    const auto pos = header_line.find(':');
    if (pos == std::string_view::npos) {
        return std::nullopt;
    }
    return trim(std::string(header_line.substr(pos + 1)));
}

}  // namespace

std::string_view to_string(HttpMethod method) noexcept {
    switch (method) {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::DELETE: return "DELETE";
        case HttpMethod::PATCH: return "PATCH";
        case HttpMethod::HEAD: return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        case HttpMethod::UNKNOWN: return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string_view to_string(HttpVersion version) noexcept {
    switch (version) {
        case HttpVersion::HTTP_1_0: return "HTTP/1.0";
        case HttpVersion::HTTP_1_1: return "HTTP/1.1";
        case HttpVersion::UNKNOWN: return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string_view to_string(ParseError error) noexcept {
    switch (error) {
        case ParseError::None: return "None";
        case ParseError::InvalidRequestLine: return "InvalidRequestLine";
        case ParseError::InvalidMethod: return "InvalidMethod";
        case ParseError::InvalidTarget: return "InvalidTarget";
        case ParseError::InvalidVersion: return "InvalidVersion";
        case ParseError::InvalidHeader: return "InvalidHeader";
        case ParseError::HeaderTooLarge: return "HeaderTooLarge";
        case ParseError::RequestLineTooLarge: return "RequestLineTooLarge";
        case ParseError::BodyTooLarge: return "BodyTooLarge";
        case ParseError::InvalidContentLength: return "InvalidContentLength";
        case ParseError::IncompleteRequest: return "IncompleteRequest";
    }
    return "Unknown";
}

HttpMethod method_from_string(std::string_view method) noexcept {
    const auto value = lowercase(std::string(method));
    if (value == "get") return HttpMethod::GET;
    if (value == "post") return HttpMethod::POST;
    if (value == "put") return HttpMethod::PUT;
    if (value == "delete") return HttpMethod::DELETE;
    if (value == "patch") return HttpMethod::PATCH;
    if (value == "head") return HttpMethod::HEAD;
    if (value == "options") return HttpMethod::OPTIONS;
    return HttpMethod::UNKNOWN;
}

HttpVersion version_from_string(std::string_view version) noexcept {
    if (version == "HTTP/1.0") return HttpVersion::HTTP_1_0;
    if (version == "HTTP/1.1") return HttpVersion::HTTP_1_1;
    return HttpVersion::UNKNOWN;
}

HttpParser::HttpParser(ParseLimits limits) : limits_(limits) {}

const ParseLimits& HttpParser::limits() const noexcept { return limits_; }

ParseResult HttpParser::parse(std::string_view data) const {
    ParseResult result;
    if (data.empty()) {
        result.error = ParseError::IncompleteRequest;
        return result;
    }

    const auto request_line_end = data.find("\r\n");
    if (request_line_end == std::string_view::npos) {
        if (data.size() > limits_.max_request_line_size) {
            result.error = ParseError::RequestLineTooLarge;
            return result;
        }
        result.error = ParseError::IncompleteRequest;
        return result;
    }

    if (request_line_end > limits_.max_request_line_size) {
        result.error = ParseError::RequestLineTooLarge;
        return result;
    }

    const std::string_view request_line = data.substr(0, request_line_end);
    const auto method_end = request_line.find(' ');
    const auto target_end = request_line.rfind(' ');
    if (method_end == std::string_view::npos || target_end == std::string_view::npos || method_end == target_end) {
        result.error = ParseError::InvalidRequestLine;
        return result;
    }

    const std::string_view method_token = request_line.substr(0, method_end);
    const std::string_view target_token = request_line.substr(method_end + 1, target_end - method_end - 1);
    const std::string_view version_token = request_line.substr(target_end + 1);

    const HttpMethod method = method_from_string(method_token);
    if (method == HttpMethod::UNKNOWN) {
        result.error = ParseError::InvalidMethod;
        return result;
    }

    if (target_token.empty() || target_token[0] != '/') {
        result.error = ParseError::InvalidTarget;
        return result;
    }

    const HttpVersion version = version_from_string(version_token);
    if (version == HttpVersion::UNKNOWN) {
        result.error = ParseError::InvalidVersion;
        return result;
    }

    HttpRequest request(method, std::string(target_token), version);
    std::size_t cursor = request_line_end + 2;
    std::size_t header_bytes = 0;
    std::string content_length_value;

    while (cursor < data.size()) {
        const auto header_end = data.find("\r\n", cursor);
        if (header_end == std::string_view::npos) {
            break;
        }

        const std::string_view header_line = data.substr(cursor, header_end - cursor);
        cursor = header_end + 2;

        if (header_line.empty()) {
            break;
        }

        const auto colon = header_line.find(':');
        if (colon == std::string_view::npos) {
            result.error = ParseError::InvalidHeader;
            return result;
        }

        const std::string name = trim(std::string(header_line.substr(0, colon)));
        const std::string value = trim(std::string(header_line.substr(colon + 1)));
        if (name.empty()) {
            result.error = ParseError::InvalidHeader;
            return result;
        }

        header_bytes += header_line.size() + 2;
        if (header_bytes > limits_.max_header_size) {
            result.error = ParseError::HeaderTooLarge;
            return result;
        }

        request.headers().set(name, value);
        if (lowercase(name) == "content-length") {
            content_length_value = value;
        }
    }

    if (cursor > data.size()) {
        result.error = ParseError::IncompleteRequest;
        return result;
    }

    if (data.compare(cursor, 2, "\r\n") == 0) {
        cursor += 2;
    }

    std::size_t body_start = cursor;
    std::size_t expected_body_size = 0;
    if (!content_length_value.empty()) {
        try {
            expected_body_size = static_cast<std::size_t>(std::stoull(content_length_value));
        } catch (...) {
            result.error = ParseError::InvalidContentLength;
            return result;
        }

        if (expected_body_size > limits_.max_body_size) {
            result.error = ParseError::BodyTooLarge;
            return result;
        }

        if (data.size() - body_start < expected_body_size) {
            result.error = ParseError::IncompleteRequest;
            return result;
        }

        auto body = std::string(data.substr(body_start, expected_body_size));
        request.set_body(std::move(body));
        result.consumed = body_start + expected_body_size;
    } else {
        result.consumed = body_start;
    }

    if (result.consumed > data.size()) {
        result.error = ParseError::IncompleteRequest;
        return result;
    }

    if (result.consumed == 0 && !request.headers().contains("Host") && request.version() == HttpVersion::HTTP_1_1) {
        result.error = ParseError::IncompleteRequest;
        return result;
    }

    result.request = std::move(request);
    result.error = ParseError::None;
    return result;
}

}  // namespace http
