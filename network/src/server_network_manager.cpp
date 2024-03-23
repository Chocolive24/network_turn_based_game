#include "server_network_manager.h"

#include <SFML/Network/TcpSocket.hpp>

#include <iostream>

ReturnStatus ServerNetworkManager::ListenToPort(unsigned short port) noexcept {
  if (listener_.listen(port) != sf::Socket::Done) {
    std::cerr << "Could not listen to port : " << port << '.' << '\n';
    return ReturnStatus::kFailure;
  }

  listener_.setBlocking(false);

  std::cout << "Server is listening to port : " << port << '\n';

  socket_selector_.add(listener_);

  return ReturnStatus::kSuccess;
}

bool ServerNetworkManager::WaitForNetworkEvent(const float timeout) noexcept {
  return socket_selector_.wait(sf::seconds(timeout));
}

bool ServerNetworkManager::AcceptNewConnection() noexcept {
  if (socket_selector_.isReady(listener_)) {
    clients_.emplace_back(std::make_unique<sf::TcpSocket>());
    const auto& client = clients_.back();
    if (listener_.accept(*client) != sf::Socket::Done) {
      std::cerr << "Could not accept client.\n";
    }

    client->setBlocking(false);
    socket_selector_.add(*client);

    std::cout << "Client " << client->getRemoteAddress() << ':'
              << client->getRemotePort() << " is connected. \n";

    return true;
  }

  return false;
}

void ServerNetworkManager::SendPacket(sf::Packet* packet,
                                      const ClientId client_id) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  const auto& socket = clients_[client_id];
  do {
    status = socket->send(*packet);
  } while (status == sf::Socket::Partial);

  if (status != sf::Socket::Done) {
    std::cerr << "Could not send packet to client.\n";
  }
}

PacketType ServerNetworkManager::ReceivePackets(sf::Packet* packet, 
    const ClientId client_id) noexcept {
  PacketType packet_type = PacketType::KNotReady;
  const auto client = clients_[client_id].get();

  if (client == nullptr) {
    return PacketType::KNotReady;
  }

  if (socket_selector_.isReady(*client)) {
    sf::Socket::Status status = sf::Socket::Partial;
    do {
      status = client->receive(*packet);
    } while (status == sf::Socket::Partial);

    if (status == sf::Socket::Disconnected) {
      clients_[client_id].reset();
    }

    if (status != sf::Socket::Done) {
      std::cerr << "Could not receive packet from client.\n";
    }

    *packet >> packet_type;
  }

  return packet_type;
}
