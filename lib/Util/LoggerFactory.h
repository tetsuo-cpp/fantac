#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <string>

namespace fantac::util {

// TODO: Add configuration to toggle between file and stdout.
struct LoggerConfig {
  spdlog::level::level_enum LogLevel;
};

class LoggerFactory {
public:
  LoggerFactory(const LoggerConfig &Config)
      : Sink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()),
        Config(Config) {}
  virtual ~LoggerFactory() = default;

  std::unique_ptr<spdlog::logger> createLogger(const std::string &Name) {
    auto Logger = std::make_unique<spdlog::logger>(Name, Sink);
    Logger->set_level(Config.LogLevel);
    return Logger;
  }

private:
  spdlog::sink_ptr Sink;
  LoggerConfig Config;
};

} // namespace fantac::util
