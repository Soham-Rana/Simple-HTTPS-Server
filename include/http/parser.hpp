#pragma once

#include "http/http_types.hpp"
#include "http/request.hpp"

#include <cstddef>
#include <optional>
#include <string_view>

namespace http {

struct ParseResult {
std::optional<HttpRequest> request;
ParseError error{ParseError::None};
std::size_t consumed{0};

[[nodiscard]]
bool success() const noexcept {
    return request.has_value() && error == ParseError::None;
}

[[nodiscard]]
bool complete() const noexcept {
    return success() || error != ParseError::IncompleteRequest;
}


};

class HttpParser {
public:
explicit HttpParser(
ParseLimits limits = {}
);

[[nodiscard]]
ParseResult parse(std::string_view data) const;

[[nodiscard]]
const ParseLimits& limits() const noexcept;


private:
ParseLimits limits_;
};

} // namespace http