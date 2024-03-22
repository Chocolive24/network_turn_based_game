#pragma once

#include "lobby.h"
#include "server_network_manager.h"

#include <SFML/Network.hpp>

#include <vector>

class Server {
 public:
  explicit Server(ServerNetworkInterface* server_net_interface) noexcept;
  [[noreturn]] void Run() noexcept;

 private:
  static constexpr std::uint8_t kStartLobbyCount = 10;

  ServerNetworkInterface* server_network_interface_ = nullptr;

  std::vector<Lobby> lobbies_{};
  std::size_t client_count_ = 0;
};