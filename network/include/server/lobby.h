#pragma once

#include "types.h"

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddClient(Port port) noexcept;
  void Clear() noexcept;

  Port client_1_port = 0;
  Port client_2_port = 0;
};