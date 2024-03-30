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


private:
  void OnPacketReceived(ClientPacket* client_packet) noexcept;
  void OnClientDisconnection(Port client_port) noexcept;

  void AddClientToLobby(const ClientData& client_data) noexcept;
  Port FindClientOpponentInLobbies(Port client_port) const noexcept;
  bool UpdatePlayerElo(const ClientData& client_data, int elo_gain) const;
  void IdentifyClient(const ClientData& client_data) const noexcept;

  ServerNetworkInterface* server_network_interface_ = nullptr;
  HttpInterface* http_interface_ = nullptr;

  std::vector<Lobby> lobbies_{};
  static constexpr std::uint8_t kStartLobbyCount = 10;
};
