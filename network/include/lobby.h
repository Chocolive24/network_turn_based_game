#pragma once

#include "types.h"

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddPlayer(ClientPort id) noexcept;

  ClientPort client_1_id = 0;
  ClientPort client_2_id = 0;
};