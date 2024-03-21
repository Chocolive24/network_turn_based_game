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