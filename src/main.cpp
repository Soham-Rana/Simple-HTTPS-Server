#include "http/request.hpp"
#include "http/response.hpp"
#include "server/server.hpp"
#include "server/server_config.hpp"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {

std::string url_decode(std::string_view value) {
    std::string decoded;
    decoded.reserve(value.size());

    for (std::size_t index = 0; index < value.size(); ++index) {
        if (value[index] == '+') {
            decoded.push_back(' ');
            continue;
        }

        if (value[index] == '%' && index + 2 < value.size() &&
            std::isxdigit(static_cast<unsigned char>(value[index + 1])) &&
            std::isxdigit(static_cast<unsigned char>(value[index + 2]))) {
            const auto hex_value = [](char character) {
                if (character >= '0' && character <= '9') return character - '0';
                if (character >= 'a' && character <= 'f') return character - 'a' + 10;
                return character - 'A' + 10;
            };
            decoded.push_back(static_cast<char>(hex_value(value[index + 1]) * 16 + hex_value(value[index + 2])));
            index += 2;
            continue;
        }

        decoded.push_back(value[index]);
    }

    return decoded;
}

std::string html_escape(std::string_view value) {
    std::string escaped;
    escaped.reserve(value.size());

    for (const char character : value) {
        switch (character) {
            case '&': escaped += "&amp;"; break;
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            case '\n': escaped += "<br>"; break;
            case '\r': break;
            default: escaped.push_back(character); break;
        }
    }

    return escaped;
}

std::string form_value(std::string_view body, std::string_view name) {
    std::size_t start = 0;
    while (start <= body.size()) {
        const auto end = body.find('&', start);
        const auto field = body.substr(start, end == std::string_view::npos ? body.size() - start : end - start);
        const auto separator = field.find('=');
        if (separator != std::string_view::npos && field.substr(0, separator) == name) {
            return url_decode(field.substr(separator + 1));
        }
        if (end == std::string_view::npos) break;
        start = end + 1;
    }
    return {};
}

http::HttpResponse health_handler(const http::HttpRequest&) {
    auto response = http::HttpResponse::ok(R"({"status":"ok"})");
    response.set_content_type("application/json");
    return response;
}

http::HttpResponse root_handler(const http::HttpRequest&) {
    auto response = http::HttpResponse::ok(
        "<!doctype html><html><head><meta charset=\"utf-8\"><title>CPP Server</title></head>"
        "<body><h1>CPP Server</h1><form method=\"post\" action=\"/print\">"
        "<label for=\"message\">What should be printed?</label><br>"
        "<textarea id=\"message\" name=\"message\" rows=\"5\" cols=\"50\" required></textarea><br>"
        "<button type=\"submit\">Print</button></form></body></html>");
    response.set_content_type("text/html; charset=utf-8");
    return response;
}

http::HttpResponse print_handler(const http::HttpRequest& request) {
    const auto message = form_value(request.body(), "message");
    auto response = http::HttpResponse::ok(
        "<!doctype html><html><head><meta charset=\"utf-8\"><title>Printed text</title></head>"
        "<body><h1>Your text</h1><p>" + html_escape(message) +
        "</p><p><a href=\"/\">Print another message</a></p></body></html>");
    response.set_content_type("text/html; charset=utf-8");
    return response;
}

}  // namespace

int main() {
    try {
        server::ServerConfig config;
        config.host = "0.0.0.0";
        config.port = 8443;
        config.worker_threads = 4;
        config.max_connections = 1024;
        config.tls_enabled = true;
        config.certificate_file = "certs/server.crt";
        config.private_key_file = "certs/server.key";
        config.validate();

        server::Server server(config);
        server.router().get("/", root_handler);
        server.router().get("/health", health_handler);
        server.router().post("/print", print_handler);

        std::cout << "Starting server on " << config.host << ':' << config.port << '\n';
        server.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
