#pragma once

#include <cstdint>
#include <cstddef>

enum class ReturnStatus : std::int16_t {
  kSuccess = 0, kFailure = 1
};

using ClientId = std::size_t;