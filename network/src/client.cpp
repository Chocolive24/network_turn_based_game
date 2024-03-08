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

//void Client::SendPacket(const Packet& packet) noexcept {
//  const char* ptr = static_cast<const char*>(packet.data);
//  std::size_t total_sent = 0;
//
//  while (total_sent < packet.size) {
//    std::size_t sent = 0;
//    const auto status = socket_.send(ptr + total_sent,
//        packet.size - total_sent, sent);
//
//    switch (status) {
//      case sf::Socket::Done:
//        total_sent += sent;
//        break;
//      default:
//        std::cerr << "Could not send packet. \n";
//        return;
//    }
//  }
//}

void Client::SendPacket(sf::Packet& packet) noexcept {
  if (socket_.send(packet) != sf::Socket::Done) {
    std::cerr << "Could not send packet.\n";
  }
}

//void Client::ReceivePacket(const Packet& packet) noexcept {
//  std::size_t received = 0;
//  if (socket_.receive(packet.data, packet.size, received) != sf::Socket::Done) {
//    std::cerr << "Could not receive packet.\n";
//  }
//}

PacketType Client::ReceivePacket(sf::Packet& packet) noexcept {
  if (socket_.receive(packet) != sf::Socket::Done) {
    std::cerr << "Could not receive packet.\n";
  }

  PacketType packet_type = PacketType::kNone;
  packet >> packet_type;

  return packet_type;
}
