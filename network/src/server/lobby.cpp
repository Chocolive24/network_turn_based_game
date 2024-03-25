#include "server/lobby.h"

bool Lobby::IsComplete() const noexcept {
  return client_1_port > 0 && client_2_port > 0;
}

void Lobby::AddClient(const ClientPort port) noexcept {
  if (client_1_port == 0) {
    client_1_port = port;
  }
  else {
    client_2_port = port;
  }
}

void Lobby::Clear() noexcept {
  client_1_port = 0;
  client_2_port = 0;
}
