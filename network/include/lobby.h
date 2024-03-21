#pragma once

#include "network_interface.h"

using ClientIdx = std::size_t;

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;

  void AddPlayer(ClientIdx id) noexcept;

  ClientIdx client_1_id = -1;
  ClientIdx client_2_id = -1;
};