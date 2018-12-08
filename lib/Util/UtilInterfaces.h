#pragma once

#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <string>

namespace fantac::util {

class ILoggerFactory {
public:
  virtual ~ILoggerFactory() = default;

  virtual std::unique_ptr<spdlog::logger>
  createLogger(const std::string &Name) = 0;
};

} // namespace fantac::util
