#pragma once

#include <SFML/Network/Packet.hpp>

enum class PacketType : std::int16_t {
  kNone = 0,
  KNotReady,
  KStartGame,
  kNewTurn,
  kForceAppliedToBall,
  kBallPositionsPacket,
};

inline sf::Packet& operator<<(sf::Packet& packet, const PacketType& type) {
  return packet << static_cast<std::int16_t>(type);
}

inline sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
  std::int16_t value;
  packet >> value;
  type = static_cast<PacketType>(value);
  return packet;
}