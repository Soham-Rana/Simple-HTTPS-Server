#pragma once

#include "http/request.hpp"
#include "http/response.hpp"

#include <functional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace http {

using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;

class Router {
public:
Router() = default;

void get(std::string path, RequestHandler handler);

void post(std::string path, RequestHandler handler);

void put(std::string path, RequestHandler handler);

void patch(std::string path, RequestHandler handler);

void delete_route(std::string path, RequestHandler handler);

void head(std::string path, RequestHandler handler);

void options(std::string path, RequestHandler handler);

[[nodiscard]]
HttpResponse route(const HttpRequest& request) const;

[[nodiscard]]
bool contains(
    HttpMethod method,
    std::string_view path
) const;

void clear();


private:
struct Route {
HttpMethod method;
std::string path;
RequestHandler handler;
};

[[nodiscard]]
const Route* find_route(
    HttpMethod method,
    std::string_view path
) const;

void add_route(
    HttpMethod method,
    std::string path,
    RequestHandler handler
);

std::vector<Route> routes_;

mutable std::shared_mutex mutex_;


};

} // namespace http