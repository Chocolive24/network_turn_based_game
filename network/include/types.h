#pragma once

#include <cstdint>

enum class ReturnStatus : std::int8_t {
  kSuccess = 0,
  kFailure = 1
};

using ClientPort = std::uint16_t;