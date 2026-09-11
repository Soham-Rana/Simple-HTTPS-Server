#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace http {

enum class HttpMethod {
GET,
POST,
PUT,
DELETE,
PATCH,
HEAD,
OPTIONS,
UNKNOWN
};

enum class HttpVersion {
HTTP_1_0,
HTTP_1_1,
UNKNOWN
};

enum class ParseError {
None,
InvalidRequestLine,
InvalidMethod,
InvalidTarget,
InvalidVersion,
InvalidHeader,
HeaderTooLarge,
RequestLineTooLarge,
BodyTooLarge,
InvalidContentLength,
IncompleteRequest
};

struct ParseLimits {
std::size_t max_request_line_size{8192};
std::size_t max_header_size{16384};
std::size_t max_body_size{10 * 1024 * 1024};
};

[[nodiscard]]
std::string_view to_string(HttpMethod method) noexcept;

[[nodiscard]]
std::string_view to_string(HttpVersion version) noexcept;

[[nodiscard]]
std::string_view to_string(ParseError error) noexcept;

[[nodiscard]]
HttpMethod method_from_string(std::string_view method) noexcept;

[[nodiscard]]
HttpVersion version_from_string(std::string_view version) noexcept;

} // namespace http