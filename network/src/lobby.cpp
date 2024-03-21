#include "lobby.h"

bool Lobby::IsComplete() const noexcept {
  return player_1 != nullptr && player_2 != nullptr; 
}

void Lobby::AddPlayer(std::unique_ptr<NetworkInterface>&& player) noexcept {
  if (player_1 == nullptr) {
    player_1 = std::move(player);
  }
  else {
    player_2 = std::move(player);
  }
}