#include "client.h"

ReturnStatus Client::ConnectToServer(const sf::IpAddress& remote_address,
                                     const unsigned short remote_port, bool blocking) noexcept {
  const sf::Socket::Status status =
      socket_.connect(remote_address, remote_port);
  if (status != sf::Socket::Done) {
    std::cerr << "Could not connect to the server.\n";
    return ReturnStatus::kFailure;
  }

  socket_.setBlocking(blocking);

  return ReturnStatus::kSuccess;
}

void Client::SendPacket(sf::Packet& packet) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  do {
    status = socket_.send(packet);
  } while (status == sf::Socket::Partial);

  //socket_.send(packet);
}

PacketType Client::ReceivePacket(sf::Packet& packet) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  do {
    status = socket_.receive(packet);
  } while (status == sf::Socket::Partial);

 /* if (status != sf::Socket::Done) {
    std::cerr << "CLIENT NO Could not receive packet from client.\n";
  }*/

  //if (socket_.receive(packet) == sf::Socket::NotReady) {
  //  return PacketType::KNotReady;
  //}

  PacketType packet_type = PacketType::kNone;
  packet >> packet_type;

  return packet_type;
}
