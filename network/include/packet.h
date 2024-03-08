#pragma once

#include <cstddef>
#include <SFML/Network/Packet.hpp>

enum class PacketType : std::int16_t {
  kNone = 0,
  KNotReady,
  KStartGame,
  kForceAppliedToBall,
};

struct Packet final : sf::Packet {
  Packet(PacketType packet_type) noexcept : type(packet_type) {}

  PacketType type = PacketType::kNone;
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


//class Packet {
//public:
//  Packet(void* data, const std::size_t& size) noexcept;
//
//  void* data = nullptr;
//  std::size_t size = 0;
//};