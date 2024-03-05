#pragma once

#include "constants.h"
#include "packet.h"

#include <SFML/Network.hpp>

#include <iostream>



class Client {
public:
  ReturnStatus ConnectToServer(const sf::IpAddress& remote_address, 
	                             unsigned short remote_port, bool blocking = true) noexcept;

  void SendPacket(const Packet& packet) noexcept;
  void SendPacket(sf::Packet& packet) noexcept;
  void ReceivePacket(const Packet& packet) noexcept;
  void ReceivePacket(sf::Packet& packet) noexcept;

  [[nodiscard]] sf::IpAddress remote_address() const noexcept {
    return socket_.getRemoteAddress();
  }
  [[nodiscard]] unsigned short remote_port() const noexcept {
    return socket_.getRemotePort();
  }

  void SetBlocking(const bool blocking) noexcept { socket_.setBlocking(blocking); }

private:
 sf::TcpSocket socket_{};
};