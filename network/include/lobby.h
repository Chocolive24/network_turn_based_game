#pragma once

#include "types.h"

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddPlayer(ClientId id) noexcept;

  ClientId client_1_id = -1;
  ClientId client_2_id = -1;
};