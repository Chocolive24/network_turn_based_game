#pragma once

#include "types.h"
#include "server_network_interface.h"

#include <SFML/Network/SocketSelector.hpp>
#include <SFML/Network/TcpListener.hpp>

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

  [[nodiscard]] bool WaitForNetworkEvent(float timeout) noexcept override;
  [[nodiscard]] bool AcceptNewConnection() noexcept override;
  void SendPacket(sf::Packet* packet, ClientPort client_id) noexcept override;
  void PollClientPackets() noexcept override;

private:
  std::vector<std::unique_ptr<sf::TcpSocket>> clients_{};
  sf::TcpListener listener_{};
  sf::SocketSelector socket_selector_{};
};