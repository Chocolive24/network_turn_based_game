#pragma once

#include "types.h"

struct ClientData {
  std::string username{};
  Port port = 0;
};

class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddClient(const ClientData& client_data) noexcept;
  void Clear() noexcept;

  ClientData client_data_1;
  ClientData client_data_2;
};