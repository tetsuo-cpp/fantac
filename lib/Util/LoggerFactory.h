#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <string>

namespace fantac::util {

class LoggerFactory {
public:
  LoggerFactory()
      : Sink(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()) {}
  virtual ~LoggerFactory() = default;

  spdlog::logger createLogger(const std::string &Name) {
    return spdlog::logger(Name, Sink);
  }

private:
  spdlog::sink_ptr Sink;
};

} // namespace fantac::util
