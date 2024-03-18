#pragma once

#include <SFML/Network/Packet.hpp>

enum class PacketType : std::uint8_t {
  kNone = 0,
  KNotReady,
  KStartGame,
  kNewTurn,
  KCueBallVelocity,
  kBallStateCorrections,
};

sf::Packet& operator<<(sf::Packet& packet, const PacketType& type);
sf::Packet& operator>>(sf::Packet& packet, PacketType& type);

/**
 * \brief PacketCommunicationInterface is an interface for sending and
 * receiving packets.
 */
class PacketCommunicationInterface {
public:
  PacketCommunicationInterface() noexcept = default;
  PacketCommunicationInterface(PacketCommunicationInterface&& other)
    noexcept = default;
  PacketCommunicationInterface& operator=(
      PacketCommunicationInterface&& other) noexcept = default;
  PacketCommunicationInterface(
      const PacketCommunicationInterface& other) noexcept = default;
  PacketCommunicationInterface& operator=(
      const PacketCommunicationInterface& other) noexcept = default;
  virtual ~PacketCommunicationInterface() noexcept = default;

  virtual void SendPacket(sf::Packet& packet) noexcept = 0;
  [[nodiscard]] virtual PacketType ReceivePacket(sf::Packet& packet) noexcept = 0;
};