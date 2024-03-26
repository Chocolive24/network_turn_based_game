#pragma once

#include "types.h"

#include <SFML/Network/Packet.hpp>

enum class PacketType : std::int8_t {
  kNone = 0,
  KNotReady,
  kClientIdentification,
  kJoinLobby,
  KStartGame,
  kNewTurn,
  KCueBallVelocity,
  kBallStateCorrections,
  kGameWon,
  kGameLost,
  kEloUpdated,
};

/**
 * \brief ClientPacket is an object containing data that a client wishes to send
 * to the server and its remote port.
 */
struct ClientPacket {
  sf::Packet data{};
  Port client_port = 0;
};

sf::Packet& operator<<(sf::Packet& packet, const PacketType& type);
sf::Packet& operator>>(sf::Packet& packet, PacketType& type);