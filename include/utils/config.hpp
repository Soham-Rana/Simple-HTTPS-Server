#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace utils {

/**

Simple INI-style configuration reader.

Configuration format:

[section]

key = value

Lines beginning with '#' or ';' are treated as comments.
*/
class Config {
public:
Config() = default;

/**

Load configuration from a file.
@throws std::runtime_error if the file cannot be opened or parsed.
*/
void load(const std::string& filename);

/**

Load configuration from a string.
*/
void load_from_string(std::string_view contents);

/**

Check whether a configuration key exists.
*/
[[nodiscard]]
bool contains(
std::string_view section,
std::string_view key
) const;

/**

Get a string configuration value.
@throws std::out_of_range if the key does not exist.
*/
[[nodiscard]]
std::string get(
std::string_view section,
std::string_view key
) const;

/**

Get a configuration value or return a fallback.
*/
[[nodiscard]]
std::string get_or(
std::string_view section,
std::string_view key,
std::string_view fallback
) const;

/**

Get an integer configuration value.
@throws std::runtime_error if the value is not a valid integer.
*/
[[nodiscard]]
int get_int(
std::string_view section,
std::string_view key
) const;

/**

Get an integer configuration value or fallback.
*/
[[nodiscard]]
int get_int_or(
std::string_view section,
std::string_view key,
int fallback
) const;

/**

Get a boolean configuration value.
Accepted values:
true, false
yes, no
on, off
1, 0
*/
[[nodiscard]]
bool get_bool(
std::string_view section,
std::string_view key
) const;

/**

Get a boolean configuration value or fallback.
*/
[[nodiscard]]
bool get_bool_or(
std::string_view section,
std::string_view key,
bool fallback
) const;

/**

Set or replace a configuration value.
*/
void set(
std::string section,
std::string key,
std::string value
);

/**

Remove all configuration values.
*/
void clear() noexcept;

/**

Returns the number of stored configuration values.
*/
[[nodiscard]]
std::size_t size() const noexcept;

private:
using KeyValueMap = std::unordered_map<std::string, std::string>;

/**
 * Build the internal key used for section/key lookup.
 */
[[nodiscard]]
static std::string make_key(
    std::string_view section,
    std::string_view key
);

/**
 * Remove leading/trailing whitespace.
 */
[[nodiscard]]
static std::string trim(std::string_view value);

/**
 * Parse one configuration line.
 */
void parse_line(
    std::string_view line,
    std::string& current_section,
    std::size_t line_number
);

KeyValueMap values_;


};

} // namespace utils