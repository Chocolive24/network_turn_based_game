#pragma once

#include "types.h"
#include "network_interface.h"

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>

#include <memory>

class ServerNetworkInterface {
public:
  constexpr ServerNetworkInterface() noexcept = default;
  constexpr ServerNetworkInterface(ServerNetworkInterface&& other) noexcept = default;
  constexpr ServerNetworkInterface& operator=(
      ServerNetworkInterface&& other) noexcept = default;
  constexpr ServerNetworkInterface(
      const ServerNetworkInterface& other) noexcept = default;
  constexpr ServerNetworkInterface& operator=(
      const ServerNetworkInterface& other) = default;
  virtual ~ServerNetworkInterface() noexcept = default;

  [[nodiscard]] virtual bool WaitForNetworkEvent(float timeout) noexcept = 0;
  [[nodiscard]] virtual bool AcceptNewConnection() noexcept = 0;
  virtual void SendPacket(sf::Packet* packet, ClientId client_id) noexcept = 0;
  [[nodiscard]] virtual PacketType ReceivePacket(sf::Packet* packet, 
      ClientId client_id) noexcept = 0;
};

/**
 * \brief ServerNetworkManager is an implementation of the NetworkInterface which
 * manages network communications between the server and clients.
 */
class ServerNetworkManager final : public ServerNetworkInterface {
public:
  ServerNetworkManager() noexcept = default;
  ServerNetworkManager(ServerNetworkManager&& other) noexcept = delete;
  ServerNetworkManager& operator=(ServerNetworkManager&& other) noexcept = delete;
  ServerNetworkManager(const ServerNetworkManager& other) noexcept = delete;
  ServerNetworkManager& operator=(const ServerNetworkManager& other)
    noexcept = delete;
  ~ServerNetworkManager() noexcept override = default;

  [[nodiscard]] ReturnStatus ListenToPort(unsigned short port) noexcept;

  [[nodiscard]] bool WaitForNetworkEvent(float timeout) noexcept override;
  [[nodiscard]] bool AcceptNewConnection() noexcept;
  void SendPacket(sf::Packet* packet, ClientId client_id) noexcept override;
  [[nodiscard]] PacketType ReceivePacket(sf::Packet* packet, ClientId client_id) noexcept override;

private:
  std::vector<std::unique_ptr<sf::TcpSocket>> clients_{};
  sf::TcpListener listener_{};
  sf::SocketSelector socket_selector_{};
};