#pragma once

#include "types.h"
#include "network_interface.h"

#include <SFML/Network.hpp>

#include <iostream>

//TODO: rework parce que cette classe est une socket implémenté comme un client.
// TODO: le server ne devrait pas utiliser cette class.

class ClientSocket final : public NetworkInterface {
public:
  ~ClientSocket() noexcept override = default;

  ReturnStatus ConnectToServer(const sf::IpAddress& remote_address, 
	                             unsigned short remote_port, bool blocking = true) noexcept;

  void SendPacket(sf::Packet& packet) noexcept override;
  [[nodiscard]] PacketType ReceivePacket(sf::Packet& packet) noexcept override;

  [[nodiscard]] sf::IpAddress remote_address() const noexcept {
    return socket_.getRemoteAddress();
  }
  [[nodiscard]] unsigned short remote_port() const noexcept {
    return socket_.getRemotePort();
  }

private:
 sf::TcpSocket socket_{};
};