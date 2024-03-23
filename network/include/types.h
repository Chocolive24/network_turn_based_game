#pragma once

#include <cstdint>

enum class ReturnStatus : std::int16_t {
  kSuccess = 0, kFailure = 1
};

using ClientPort = std::uint16_t;