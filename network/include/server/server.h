#pragma once

#include "lobby.h"
#include "http_interface.h"
#include "server_network_interface.h"

#include <vector>

class Server {
 public:
  Server(ServerNetworkInterface* server_net_interface, 
	       HttpInterface* http_interface) noexcept;
  [[noreturn]] void Run() noexcept;
  bool UpdatePlayerElo(std::string username, Port client_port, int elo_gain);

private:
  void OnPacketReceived(ClientPacket* client_packet) noexcept;
  void OnClientDisconnection(Port client_port) noexcept;

  void AddClientToLobby(Port client_port, std::string_view username) noexcept;

  ServerNetworkInterface* server_network_interface_ = nullptr;
  HttpInterface* http_interface_ = nullptr;

  std::vector<Lobby> lobbies_{};
  static constexpr std::uint8_t kStartLobbyCount = 10;
};
