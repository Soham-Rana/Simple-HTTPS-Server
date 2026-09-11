#pragma once

#include "http/request.hpp"

#include <cstddef>
#include <string>
#include <string_view>

namespace http {

enum class StatusCode : int {
Continue = 100,

OK = 200,
Created = 201,
NoContent = 204,

BadRequest = 400,
Unauthorized = 401,
Forbidden = 403,
NotFound = 404,
MethodNotAllowed = 405,
RequestTimeout = 408,
PayloadTooLarge = 413,
URITooLong = 414,

InternalServerError = 500,
NotImplemented = 501,
HTTPVersionNotSupported = 505


};

[[nodiscard]]
std::string_view reason_phrase(StatusCode status) noexcept;

class HttpResponse {
public:
HttpResponse() = default;

explicit HttpResponse(StatusCode status);

[[nodiscard]]
StatusCode status() const noexcept;

void set_status(StatusCode status) noexcept;

void set_body(std::string body);

void set_content_type(std::string content_type);

void set_content_length(std::size_t length);

void set_keep_alive(bool keep_alive) noexcept;

void set_header(std::string name, std::string value);

[[nodiscard]]
const Headers& headers() const noexcept;

[[nodiscard]]
Headers& headers() noexcept;

[[nodiscard]]
std::string_view body() const noexcept;

[[nodiscard]]
bool keep_alive() const noexcept;

[[nodiscard]]
std::string serialize(
    HttpVersion version = HttpVersion::HTTP_1_1
) const;

static HttpResponse ok(std::string body);

static HttpResponse created(std::string body);

static HttpResponse no_content();

static HttpResponse bad_request(
    std::string body = "Bad Request"
);

static HttpResponse not_found(
    std::string body = "Not Found"
);

static HttpResponse method_not_allowed(
    std::string body = "Method Not Allowed"
);

static HttpResponse internal_server_error(
    std::string body = "Internal Server Error"
);


private:
StatusCode status_{StatusCode::OK};
Headers headers_;
std::string body_;
bool keep_alive_{true};
};

} // namespace http