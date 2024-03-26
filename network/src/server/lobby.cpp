#include "server/lobby.h"

bool Lobby::IsComplete() const noexcept {
  return client_1_port > 0 && client_2_port > 0;
}

void Lobby::AddClient(const Port port, std::string_view username) noexcept {
  if (client_1_port == 0) {
    client_1_port = port;
    username_1 = username;
  }
  else {
    client_2_port = port;
    username_2 = username;
  }
}

void Lobby::Clear() noexcept {
  client_1_port = 0;
  client_2_port = 0;
}
