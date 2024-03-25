#pragma once

#include <cstdint>
#include <string>

enum class ReturnStatus : std::int8_t {
  kSuccess = 0,
  kFailure = 1
};

struct PlayerData {
  std::string username;
  int elo;
};

using Port = std::uint16_t;