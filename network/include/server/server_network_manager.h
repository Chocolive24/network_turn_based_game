#pragma once

#include "types.h"
#include "server_network_interface.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include <memory>

/**
 * \brief ServerNetworkManager is an implementation of the NetworkInterface which
 * manages network communications between the server and the clients.
 */
class ServerNetworkManager final : public ServerNetworkInterface {
public:
  constexpr explicit ServerNetworkManager() noexcept = default;
  constexpr ServerNetworkManager(ServerNetworkManager&& other) noexcept = delete;
  constexpr ServerNetworkManager& operator=(ServerNetworkManager&& other)
    noexcept = delete;
  constexpr ServerNetworkManager(const ServerNetworkManager& other) noexcept = delete;
  constexpr ServerNetworkManager& operator=(const ServerNetworkManager& other)
    noexcept = delete;
  ~ServerNetworkManager() noexcept override = default;

  [[nodiscard]] ReturnStatus ListenToPort(Port port) noexcept;
  void SendPacket(sf::Packet* packet, Port client_port) noexcept override;
  void PollEvents() noexcept override;

private:
  void PollClientsConnections() noexcept;
  void PollClientsPackets() noexcept;
  void RemoveClient(std::unique_ptr<sf::TcpSocket>& client) noexcept;

  std::vector<std::unique_ptr<sf::TcpSocket>> clients_{};
  sf::SocketSelector socket_selector_{};
  sf::TcpListener listener_{};
};