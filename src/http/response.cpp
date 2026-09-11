#include "http/response.hpp"

#include <sstream>
#include <string>
#include <string_view>

namespace http {
namespace {

std::string to_lower(std::string value) {
    for (char& ch : value) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return value;
}

std::string title_case_header(std::string_view name) {
    if (name.empty()) {
        return {};
    }

    std::string result(name);
    if (result.size() > 1) {
        result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
        for (std::size_t i = 1; i < result.size(); ++i) {
            if (result[i - 1] == '-') {
                result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
            }
        }
    }
    return result;
}

}  // namespace

std::string_view reason_phrase(StatusCode status) noexcept {
    switch (status) {
        case StatusCode::Continue: return "Continue";
        case StatusCode::OK: return "OK";
        case StatusCode::Created: return "Created";
        case StatusCode::NoContent: return "No Content";
        case StatusCode::BadRequest: return "Bad Request";
        case StatusCode::Unauthorized: return "Unauthorized";
        case StatusCode::Forbidden: return "Forbidden";
        case StatusCode::NotFound: return "Not Found";
        case StatusCode::MethodNotAllowed: return "Method Not Allowed";
        case StatusCode::RequestTimeout: return "Request Timeout";
        case StatusCode::PayloadTooLarge: return "Payload Too Large";
        case StatusCode::URITooLong: return "URI Too Long";
        case StatusCode::InternalServerError: return "Internal Server Error";
        case StatusCode::NotImplemented: return "Not Implemented";
        case StatusCode::HTTPVersionNotSupported: return "HTTP Version Not Supported";
    }
    return "OK";
}

HttpResponse::HttpResponse(StatusCode status) : status_(status) {
    headers_.set("Content-Type", "text/plain; charset=utf-8");
}

StatusCode HttpResponse::status() const noexcept { return status_; }

void HttpResponse::set_status(StatusCode status) noexcept { status_ = status; }

void HttpResponse::set_body(std::string body) { body_ = std::move(body); }

void HttpResponse::set_content_type(std::string content_type) {
    headers_.set("Content-Type", std::move(content_type));
}

void HttpResponse::set_content_length(std::size_t length) {
    headers_.set("Content-Length", std::to_string(length));
}

void HttpResponse::set_keep_alive(bool keep_alive) noexcept { keep_alive_ = keep_alive; }

void HttpResponse::set_header(std::string name, std::string value) {
    headers_.set(std::move(name), std::move(value));
}

const Headers& HttpResponse::headers() const noexcept { return headers_; }

Headers& HttpResponse::headers() noexcept { return headers_; }

std::string_view HttpResponse::body() const noexcept { return body_; }

bool HttpResponse::keep_alive() const noexcept { return keep_alive_; }

std::string HttpResponse::serialize(HttpVersion version) const {
    std::ostringstream stream;
    stream << "HTTP/";
    if (version == HttpVersion::HTTP_1_0) {
        stream << "1.0";
    } else {
        stream << "1.1";
    }
    stream << ' ' << static_cast<int>(status_) << ' ' << reason_phrase(status_) << "\r\n";

    if (!headers_.contains("Content-Type")) {
        // Do not mutate the request state while the object is logically const.
        // Default headers are serialized without altering the stored map.
    }

    if (!headers_.contains("Content-Length")) {
        auto length = body_.size();
        stream << "Content-Length: " << length << "\r\n";
    } else {
        stream << "Content-Length: " << headers_.value_or("Content-Length") << "\r\n";
    }

    stream << "Connection: " << (keep_alive_ ? "keep-alive" : "close") << "\r\n";

    for (const auto& [name, value] : headers_.all()) {
        const std::string lower = to_lower(name);
        if (lower == "content-length" || lower == "content-type" || lower == "connection") {
            continue;
        }
        stream << title_case_header(name) << ": " << value << "\r\n";
    }

    stream << "Content-Type: " << headers_.value_or("Content-Type") << "\r\n";
    stream << "\r\n";
    if (!body_.empty()) {
        stream << body_;
    }
    return stream.str();
}

HttpResponse HttpResponse::ok(std::string body) {
    HttpResponse response(StatusCode::OK);
    response.set_body(std::move(body));
    response.set_content_length(response.body().size());
    return response;
}

HttpResponse HttpResponse::created(std::string body) {
    HttpResponse response(StatusCode::Created);
    response.set_body(std::move(body));
    response.set_content_length(response.body().size());
    return response;
}

HttpResponse HttpResponse::no_content() {
    HttpResponse response(StatusCode::NoContent);
    response.set_body("");
    response.set_content_length(0);
    return response;
}

HttpResponse HttpResponse::bad_request(std::string body) {
    HttpResponse response(StatusCode::BadRequest);
    response.set_body(std::move(body));
    response.set_content_length(response.body().size());
    return response;
}

HttpResponse HttpResponse::not_found(std::string body) {
    HttpResponse response(StatusCode::NotFound);
    response.set_body(std::move(body));
    response.set_content_length(response.body().size());
    return response;
}

HttpResponse HttpResponse::method_not_allowed(std::string body) {
    HttpResponse response(StatusCode::MethodNotAllowed);
    response.set_body(std::move(body));
    response.set_content_length(response.body().size());
    return response;
}

HttpResponse HttpResponse::internal_server_error(std::string body) {
    HttpResponse response(StatusCode::InternalServerError);
    response.set_body(std::move(body));
    response.set_content_length(response.body().size());
    return response;
}

}  // namespace http
