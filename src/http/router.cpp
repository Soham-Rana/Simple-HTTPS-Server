#include "http/router.hpp"

#include <algorithm>
#include <mutex>
#include <string>
#include <string_view>

namespace http {

void Router::add_route(HttpMethod method, std::string path, RequestHandler handler) {
    Route route{method, std::move(path), std::move(handler)};
    std::unique_lock lock(mutex_);
    routes_.push_back(std::move(route));
}

void Router::get(std::string path, RequestHandler handler) {
    add_route(HttpMethod::GET, std::move(path), std::move(handler));
}

void Router::post(std::string path, RequestHandler handler) {
    add_route(HttpMethod::POST, std::move(path), std::move(handler));
}

void Router::put(std::string path, RequestHandler handler) {
    add_route(HttpMethod::PUT, std::move(path), std::move(handler));
}

void Router::patch(std::string path, RequestHandler handler) {
    add_route(HttpMethod::PATCH, std::move(path), std::move(handler));
}

void Router::delete_route(std::string path, RequestHandler handler) {
    add_route(HttpMethod::DELETE, std::move(path), std::move(handler));
}

void Router::head(std::string path, RequestHandler handler) {
    add_route(HttpMethod::HEAD, std::move(path), std::move(handler));
}

void Router::options(std::string path, RequestHandler handler) {
    add_route(HttpMethod::OPTIONS, std::move(path), std::move(handler));
}

const Router::Route* Router::find_route(HttpMethod method, std::string_view path) const {
    std::shared_lock lock(mutex_);
    const auto it = std::find_if(routes_.begin(), routes_.end(), [&](const Route& route) {
        return route.method == method && route.path == path;
    });
    if (it == routes_.end()) {
        return nullptr;
    }
    return &(*it);
}

HttpResponse Router::route(const HttpRequest& request) const {
    const auto path = std::string(request.path());
    const Route* exact = find_route(request.method(), path);
    if (exact != nullptr) {
        return exact->handler(request);
    }

    std::shared_lock lock(mutex_);
    const auto any_same_path = std::find_if(routes_.begin(), routes_.end(), [&](const Route& route) {
        return route.path == path;
    });
    if (any_same_path != routes_.end()) {
        return HttpResponse::method_not_allowed("Method Not Allowed");
    }
    return HttpResponse::not_found("Not Found");
}

bool Router::contains(HttpMethod method, std::string_view path) const {
    return find_route(method, path) != nullptr;
}

void Router::clear() {
    std::unique_lock lock(mutex_);
    routes_.clear();
}

}  // namespace http
