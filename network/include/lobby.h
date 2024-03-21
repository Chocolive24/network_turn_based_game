#pragma once

#include <memory>

#include "network_interface.h"

using PlayerId = int;

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;

  void AddPlayer(std::unique_ptr<NetworkInterface>&& player) noexcept;

 std::unique_ptr<NetworkInterface> player_1 = nullptr;
 std::unique_ptr<NetworkInterface> player_2 = nullptr;
};