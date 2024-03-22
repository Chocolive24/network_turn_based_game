#pragma once

#include "packet.h"

class ServerNetworkInterface {
 public:
  constexpr ServerNetworkInterface() noexcept = default;
  constexpr ServerNetworkInterface(ServerNetworkInterface&& other) noexcept =
      default;
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
  [[nodiscard]] virtual PacketType ReceivePacket(
      sf::Packet* packet, ClientId client_id) noexcept = 0;
};