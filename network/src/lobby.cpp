#include "lobby.h"

bool Lobby::IsComplete() const noexcept {
  return client_1_id > 0 && client_2_id > 0;
}

void Lobby::AddPlayer(const ClientPort id) noexcept {
  if (client_1_id == 0) {
    client_1_id = id;
  }
  else {
    client_2_id = id;
  }
}