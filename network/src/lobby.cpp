#include "lobby.h"

bool Lobby::IsComplete() const noexcept {
  return static_cast<int>(client_1_id) > -1 &&
         static_cast<int>(client_2_id) > -1;
}

void Lobby::AddPlayer(const ClientId id) noexcept {
  if (static_cast<int>(client_1_id) == -1) {
    client_1_id = id;
  }
  else {
    client_2_id = id;
  }
}