#pragma once

#include "lobby.h"
#include "server_network_interface.h"

#include <vector>

class Server {
 public:
  explicit Server(ServerNetworkInterface* server_net_interface) noexcept;
  [[noreturn]] void Run() noexcept;

 private:
  void OnPacketReceived(ClientPacket* client_packet) noexcept;
  void OnClientDisconnection(ClientPort client_port) noexcept;

  void AddClientToLobby(ClientPort client_port) noexcept;

  ServerNetworkInterface* server_network_interface_ = nullptr;
  std::vector<Lobby> lobbies_{};

  static constexpr std::uint8_t kStartLobbyCount = 10;
};