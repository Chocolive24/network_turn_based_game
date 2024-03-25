#pragma once

#include "types.h"
#include "server_network_interface.h"

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include <memory>

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

  void SendPacket(sf::Packet* packet, ClientPort client_id) noexcept override;
  void PollEvents() noexcept override;

  void RemoveClient(std::unique_ptr<sf::TcpSocket>& client) noexcept;

private:
  void PollClientsConnections() noexcept;
  void PollClientsPackets() noexcept;
  std::vector<std::unique_ptr<sf::TcpSocket>> clients_{};
  sf::TcpListener listener_{};
  sf::SocketSelector socket_selector_{};
};