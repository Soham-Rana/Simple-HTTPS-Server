#include "net/address.hpp"

#include <sstream>
#include <utility>

namespace net {

Address::Address(std::string host, std::uint16_t port, AddressFamily family)
    : host_(std::move(host)), port_(port), family_(family) {}

const std::string& Address::host() const noexcept { return host_; }

std::uint16_t Address::port() const noexcept { return port_; }

AddressFamily Address::family() const noexcept { return family_; }

std::string Address::to_string() const {
    std::ostringstream stream;
    stream << host_;
    if (port_ != 0) {
        stream << ':' << port_;
    }
    return stream.str();
}

bool Address::empty() const noexcept {
    return host_.empty() && port_ == 0 && family_ == AddressFamily::Unspecified;
}

}  // namespace net
