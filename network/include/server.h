#pragma once

#include "constants.h"
#include "packet.h"

#include <SFML/Network.hpp>

#include <array>

#include "client.h"
#include "lobby.h"

class ServerSocket final : public NetworkInterface {
 public:
  ~ServerSocket() noexcept override = default;

  void SendPacket(sf::Packet& packet) noexcept override;
  [[nodiscard]] PacketType ReceivePacket(sf::Packet& packet) noexcept override;

  sf::TcpSocket socket{};
};

class Server {
public:
  ReturnStatus Init(unsigned short port) noexcept;
  void Run() noexcept;

 private:
  //static constexpr int kMaxClientCount_ = 2;
  static constexpr std::uint8_t kStartLobbyCount = 10;

  sf::TcpListener listener_{};
  sf::SocketSelector socket_selector_{};
  std::vector<std::unique_ptr<sf::TcpSocket>> clients_{};
  std::vector<Lobby> lobbies_{};

  //std::vector<ClientSocket> clients_{};
  //int client_index_ = 0;

  void AcceptClient() noexcept;
  void CommunicatePacketBetweenClients() noexcept;
};
