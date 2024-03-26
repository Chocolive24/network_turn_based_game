#pragma once

#include "types.h"

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddClient(Port port, std::string_view username) noexcept;
  void Clear() noexcept;

  //TODO: Struct PlayerData.
  Port client_1_port = 0;
  Port client_2_port = 0;
  std::string username_1{};
  std::string username_2{};
};