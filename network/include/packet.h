#pragma once

#include "types.h"

#include <SFML/Network/Packet.hpp>

enum class PacketType : std::uint8_t {
  kNone = 0,
  KNotReady,
  kJoinLobby,
  KStartGame,
  kNewTurn,
  KCueBallVelocity,
  kBallStateCorrections,
};

struct ClientPacket {
  sf::Packet packet_data{};
  ClientPort client_id = -1;
};

sf::Packet& operator<<(sf::Packet& packet, const PacketType& type);
sf::Packet& operator>>(sf::Packet& packet, PacketType& type);