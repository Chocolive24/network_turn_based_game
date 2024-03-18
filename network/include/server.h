#pragma once

#include "constants.h"
#include "packet.h"

#include <SFML/Network.hpp>

#include <array>

class Server {
public:
  ReturnStatus Init(unsigned short port) noexcept;
  void Run() noexcept;

 private:
  static constexpr int kMaxClientCount_ = 2;

  sf::TcpListener listener_{};
  sf::SocketSelector socket_selector_{};

  std::array<sf::TcpSocket, kMaxClientCount_> clients_{};
  int client_index_ = 0;

  void AcceptClient() noexcept;
  void CommunicatePacketBetweenClients() noexcept;
};