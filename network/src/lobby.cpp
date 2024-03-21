#include "lobby.h"

bool Lobby::IsComplete() const noexcept {
  return client_1_id != -1 && client_2_id != -1; 
}

void Lobby::AddPlayer(const ClientIdx id) noexcept {
  if (client_1_id == -1) {
    client_1_id = id;
  }
  else {
    client_2_id = id;
  }
}