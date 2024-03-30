#pragma once

#include "types.h"
#include "client_network_interface.h"

#include <SFML/Network.hpp>

/**
 * \brief ClientNetworkManager is the implementation of the ClientNetworkInterface.
 * It manages the communications with the server and owns the client's socket.
 */
class ClientNetworkManager final : public ClientNetworkInterface {
public:
  ClientNetworkManager() noexcept = default;
  ClientNetworkManager(ClientNetworkManager&& other) noexcept = delete;
  ClientNetworkManager& operator=(ClientNetworkManager&& other) noexcept = delete;
  ClientNetworkManager(const ClientNetworkManager& other) noexcept = delete;
  ClientNetworkManager& operator=(
      const ClientNetworkManager& other) noexcept = delete;
  ~ClientNetworkManager() noexcept override = default;

  void SendPacket(sf::Packet& packet) noexcept override;
  [[nodiscard]] PacketType ReceivePacket(sf::Packet& packet) noexcept override;

  ReturnStatus ConnectToServer(const sf::IpAddress& remote_address,
                               unsigned short remote_port,
                               bool blocking = true) noexcept;

  void DisconnectFromServer() noexcept;

  [[nodiscard]] sf::IpAddress remote_address() const noexcept {
    return socket_.getRemoteAddress();
  }
  [[nodiscard]] unsigned short remote_port() const noexcept {
    return socket_.getRemotePort();
  }

private:
 sf::TcpSocket socket_{};
};