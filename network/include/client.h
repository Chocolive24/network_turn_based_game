#pragma once

#include "constants.h"
#include "packet.h"

#include <SFML/Network.hpp>

#include <iostream>

class Client final : public PacketCommunicationInterface {
public:
  ~Client() noexcept override = default;

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