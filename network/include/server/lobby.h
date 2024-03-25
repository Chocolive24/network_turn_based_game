#pragma once

#include "types.h"

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddClient(ClientPort port) noexcept;
  void Clear() noexcept;

  ClientPort client_1_port = 0;
  ClientPort client_2_port = 0;
};