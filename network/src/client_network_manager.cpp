#include "client_network_manager.h"

#include <iostream>

ReturnStatus ClientNetworkManager::ConnectToServer(
    const sf::IpAddress& remote_address,
    const unsigned short remote_port, const bool blocking) noexcept {

  const sf::Socket::Status status = socket_.connect(remote_address, remote_port);
  if (status != sf::Socket::Done) {
    std::cerr << "Could not connect to the server.\n";
    return ReturnStatus::kFailure;
  }

  socket_.setBlocking(blocking);

  return ReturnStatus::kSuccess;
}

void ClientNetworkManager::DisconnectFromServer() noexcept {
  socket_.disconnect();
}

void ClientNetworkManager::SendPacket(sf::Packet& packet) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  do {
    status = socket_.send(packet);
  } while (status == sf::Socket::Partial);

  if (status != sf::Socket::Done) {
    std::cerr << "Could not send packet to client.\n";
  }
}

PacketType ClientNetworkManager::ReceivePacket(sf::Packet& packet) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  do {
    status = socket_.receive(packet);
  } while (status == sf::Socket::Partial);

  if (status == sf::Socket::NotReady) {
    return PacketType::KNotReady;
  }

  if (status != sf::Socket::Done) {
    std::cerr << "Could not receive packet from client.\n";
  }

  PacketType packet_type = PacketType::kNone;
  packet >> packet_type;

  return packet_type;
}
