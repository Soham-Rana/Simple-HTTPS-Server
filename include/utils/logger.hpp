#pragma once

#include <chrono>
#include <cstddef>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

namespace utils {

enum class LogLevel {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Off
};

[[nodiscard]]
std::string_view to_string(LogLevel level) noexcept;

[[nodiscard]]
LogLevel log_level_from_string(std::string_view level) noexcept;

/**
 * Thread-safe application logger.
 * The logger supports:
 * - Multiple log levels
 * - Console output
 * - Optional file output
 * - Thread-safe writes
 * - Timestamps
 */
class Logger {
public:
    /**
     * Create a logger writing to stderr.
     */
    explicit Logger(LogLevel minimum_level = LogLevel::Info);

    /**
     * Create a logger with optional file output.
     */
    Logger(LogLevel minimum_level, const std::string& log_file);

    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * Set the minimum log level.
     */
    void set_level(LogLevel level) noexcept;

    /**
     * Get the current minimum log level.
     */
    [[nodiscard]]
    LogLevel level() const noexcept;

    /**
     * Check whether a level will be emitted.
     */
    [[nodiscard]]
    bool enabled(LogLevel level) const noexcept;

    /**
     * Write a log message.
     */
    void log(LogLevel level, std::string_view message);

    void trace(std::string_view message);
    void debug(std::string_view message);
    void info(std::string_view message);
    void warning(std::string_view message);
    void error(std::string_view message);
    void critical(std::string_view message);

private:
    /**
     * Format a timestamp for log output.
     */
    [[nodiscard]]
    static std::string format_timestamp(std::chrono::system_clock::time_point time);

    /**
     * Write a formatted message to configured outputs.
     */
    void write(LogLevel level, std::string_view message);

    LogLevel minimum_level_{LogLevel::Info};
    std::unique_ptr<std::ofstream> file_;
    mutable std::mutex mutex_;
};

} // namespace utils