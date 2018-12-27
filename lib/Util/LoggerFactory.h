#pragma once

#include "UtilInterfaces.h"

namespace fantac::util {

inline spdlog::level::level_enum StringToLoggingLevel(const std::string &X) {
  if (X == "debug") {
    return spdlog::level::debug;
  } else if (X == "info") {
    return spdlog::level::info;
  } else if (X == "warn") {
    return spdlog::level::warn;
  }

  return spdlog::level::info;
}

// TODO: Add configuration to toggle between file and stdout.
struct LoggerConfig {
  spdlog::level::level_enum LogLevel;
};

class LoggerFactory : public ILoggerFactory {
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
