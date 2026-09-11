#include "utils/logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace utils {

std::string_view to_string(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        case LogLevel::Off: return "OFF";
    }
    return "INFO";
}

LogLevel log_level_from_string(std::string_view level) noexcept {
    if (level == "trace") return LogLevel::Trace;
    if (level == "debug") return LogLevel::Debug;
    if (level == "warning") return LogLevel::Warning;
    if (level == "error") return LogLevel::Error;
    if (level == "critical") return LogLevel::Critical;
    if (level == "off") return LogLevel::Off;
    return LogLevel::Info;
}

Logger::Logger(LogLevel minimum_level) : minimum_level_(minimum_level) {}

Logger::Logger(LogLevel minimum_level, const std::string& log_file)
    : minimum_level_(minimum_level) {
    if (!log_file.empty()) {
        file_ = std::make_unique<std::ofstream>(log_file, std::ios::out | std::ios::app);
    }
}

Logger::~Logger() = default;

void Logger::set_level(LogLevel level) noexcept { minimum_level_ = level; }

LogLevel Logger::level() const noexcept { return minimum_level_; }

bool Logger::enabled(LogLevel level) const noexcept { return level >= minimum_level_; }

void Logger::log(LogLevel level, std::string_view message) {
    if (!enabled(level)) {
        return;
    }
    write(level, message);
}

void Logger::trace(std::string_view message) { log(LogLevel::Trace, message); }
void Logger::debug(std::string_view message) { log(LogLevel::Debug, message); }
void Logger::info(std::string_view message) { log(LogLevel::Info, message); }
void Logger::warning(std::string_view message) { log(LogLevel::Warning, message); }
void Logger::error(std::string_view message) { log(LogLevel::Error, message); }
void Logger::critical(std::string_view message) { log(LogLevel::Critical, message); }

std::string Logger::format_timestamp(std::chrono::system_clock::time_point time) {
    const auto now = std::chrono::system_clock::to_time_t(time);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

void Logger::write(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> guard(mutex_);
    const auto timestamp = format_timestamp(std::chrono::system_clock::now());
    std::ostringstream out;
    out << '[' << timestamp << "] [" << to_string(level) << "] " << message << '\n';
    std::cerr << out.str();
    if (file_ && file_->is_open()) {
        *file_ << out.str();
        file_->flush();
    }
}

}  // namespace utils
