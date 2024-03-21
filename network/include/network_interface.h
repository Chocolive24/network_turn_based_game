#pragma once

#include "packet.h"

/**
 * \brief PacketCommunicationInterface is an interface for sending and
 * receiving packets.
 */
class NetworkInterface {
 public:
  NetworkInterface() noexcept = default;
  NetworkInterface(NetworkInterface&& other) noexcept =
      default;
  NetworkInterface& operator=(
      NetworkInterface&& other) noexcept = default;
  NetworkInterface(
      const NetworkInterface& other) noexcept = default;
  NetworkInterface& operator=(
      const NetworkInterface& other) noexcept = default;
  virtual ~NetworkInterface() noexcept = default;

  virtual void SendPacket(sf::Packet& packet) noexcept = 0;
  [[nodiscard]] virtual PacketType ReceivePacket(sf::Packet& packet) noexcept = 0;
};