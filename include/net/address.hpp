#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace net {

enum class AddressFamily {
IPv4,
IPv6,
Unspecified
};

class Address {
public:
Address() = default;

Address(
    std::string host,
    std::uint16_t port,
    AddressFamily family = AddressFamily::Unspecified
);

[[nodiscard]]
const std::string& host() const noexcept;

[[nodiscard]]
std::uint16_t port() const noexcept;

[[nodiscard]]
AddressFamily family() const noexcept;

[[nodiscard]]
std::string to_string() const;

[[nodiscard]]
bool empty() const noexcept;


private:
std::string host_;
std::uint16_t port_{0};
AddressFamily family_{AddressFamily::Unspecified};
};

} // namespace net