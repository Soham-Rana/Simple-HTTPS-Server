#include "utils/config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace utils {
namespace {

std::string trim(std::string_view value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string_view::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(begin, end - begin + 1));
}

std::string lower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

}  // namespace

std::string Config::make_key(std::string_view section, std::string_view key) {
    return std::string(section) + "." + std::string(key);
}

std::string Config::trim(std::string_view value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string_view::npos) {
        return {};
    }
    const auto end = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(begin, end - begin + 1));
}

void Config::load(const std::string& filename) {
    std::ifstream input(filename);
    if (!input) {
        throw std::runtime_error("Unable to open configuration file: " + filename);
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    load_from_string(buffer.str());
}

void Config::load_from_string(std::string_view contents) {
    values_.clear();
    std::string current_section;
    std::size_t line_number = 0;
    std::stringstream stream{std::string(contents)};
    std::string line;
    while (std::getline(stream, line)) {
        ++line_number;
        parse_line(line, current_section, line_number);
    }
}

bool Config::contains(std::string_view section, std::string_view key) const {
    return values_.find(make_key(section, key)) != values_.end();
}

std::string Config::get(std::string_view section, std::string_view key) const {
    const auto it = values_.find(make_key(section, key));
    if (it == values_.end()) {
        throw std::out_of_range("Missing configuration value");
    }
    return it->second;
}

std::string Config::get_or(std::string_view section, std::string_view key, std::string_view fallback) const {
    try {
        return get(section, key);
    } catch (...) {
        return std::string(fallback);
    }
}

int Config::get_int(std::string_view section, std::string_view key) const {
    const auto value = get(section, key);
    try {
        return std::stoi(value);
    } catch (...) {
        throw std::runtime_error("Invalid integer value for " + std::string(section) + "." + std::string(key));
    }
}

int Config::get_int_or(std::string_view section, std::string_view key, int fallback) const {
    try {
        return get_int(section, key);
    } catch (...) {
        return fallback;
    }
}

bool Config::get_bool(std::string_view section, std::string_view key) const {
    const auto value = lower(get(section, key));
    if (value == "true" || value == "yes" || value == "on" || value == "1") {
        return true;
    }
    if (value == "false" || value == "no" || value == "off" || value == "0") {
        return false;
    }
    throw std::runtime_error("Invalid boolean value for " + std::string(section) + "." + std::string(key));
}

bool Config::get_bool_or(std::string_view section, std::string_view key, bool fallback) const {
    try {
        return get_bool(section, key);
    } catch (...) {
        return fallback;
    }
}

void Config::set(std::string section, std::string key, std::string value) {
    values_[make_key(section, key)] = std::move(value);
}

void Config::clear() noexcept { values_.clear(); }

std::size_t Config::size() const noexcept { return values_.size(); }

void Config::parse_line(std::string_view line, std::string& current_section, std::size_t line_number) {
    const auto trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';') {
        return;
    }

    if (trimmed.front() == '[' && trimmed.back() == ']') {
        current_section = std::string(trimmed.substr(1, trimmed.size() - 2));
        return;
    }

    const auto pos = trimmed.find('=');
    if (pos == std::string_view::npos) {
        throw std::runtime_error("Invalid configuration line at " + std::to_string(line_number));
    }

    const auto key = trim(trimmed.substr(0, pos));
    const auto value = trim(trimmed.substr(pos + 1));
    if (current_section.empty()) {
        throw std::runtime_error("Configuration key without section at line " + std::to_string(line_number));
    }
    values_[make_key(current_section, key)] = std::string(value);
}

}  // namespace utils
