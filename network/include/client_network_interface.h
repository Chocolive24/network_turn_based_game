#pragma once

#include "packet.h"

/**
 * \brief ClientNetworkInterface is an interface to communicates with a server.
 */
class ClientNetworkInterface {
 public:
  ClientNetworkInterface() noexcept = default;
  ClientNetworkInterface(ClientNetworkInterface&& other) noexcept = default;
  ClientNetworkInterface& operator=(ClientNetworkInterface&& other) noexcept = default;
  ClientNetworkInterface(const ClientNetworkInterface& other) noexcept = default;
  ClientNetworkInterface& operator=(const ClientNetworkInterface& other) noexcept = default;
  virtual ~ClientNetworkInterface() noexcept = default;

  virtual void SendPacket(sf::Packet& packet) noexcept = 0;
  [[nodiscard]] virtual PacketType ReceivePacket(sf::Packet& packet) noexcept = 0;
};