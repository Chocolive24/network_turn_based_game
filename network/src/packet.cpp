#include "packet.h"

sf::Packet& operator<<(sf::Packet& packet, const PacketType& type) {
  return packet << static_cast<std::int16_t>(type);
}

sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
  std::int16_t value;
  packet >> value;
  type = static_cast<PacketType>(value);
  return packet;
}
