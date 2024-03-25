#include "server/server_network_manager.h"

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

void ServerNetworkManager::PollEvents() noexcept {
  if (socket_selector_.wait(sf::seconds(5.f))) {
    if (socket_selector_.isReady(listener_)) {
      PollClientsConnections();
    }
    else {
      PollClientsPackets();
    }
  } else {
    std::cout << "Waiting for activity...\n";
  }
}

void ServerNetworkManager::RemoveClient(std::unique_ptr<sf::TcpSocket>& client) noexcept {
  socket_selector_.remove(*client);
  if (disconnect_callback_ != nullptr) {
    disconnect_callback_(client->getRemotePort());
  }
  client.reset();
}

void ServerNetworkManager::PollClientsConnections() noexcept {
  clients_.emplace_back(std::make_unique<sf::TcpSocket>());
  const auto& client = clients_.back();

  if (listener_.accept(*client) != sf::Socket::Done) {
    std::cerr << "Could not accept client.\n";
  }

  client->setBlocking(false);
  socket_selector_.add(*client);

  std::cout << "Client " << client->getRemoteAddress() << ':'
            << client->getRemotePort() << " is connected. \n";
}

void ServerNetworkManager::PollClientsPackets() noexcept {
  ClientPacket client_packet{};
  for (auto& client : clients_) {
    if (client == nullptr) {
      continue;
    }

    if (socket_selector_.isReady(*client)) {
      sf::Socket::Status status = sf::Socket::Partial;
      do {
        status = client->receive(client_packet.data);
      } while (status == sf::Socket::Partial);

      if (status == sf::Socket::Disconnected) {
        RemoveClient(client);
        continue;
      }

      if (status != sf::Socket::Done) {
        //std::cerr << "Could not receive packet from client.\n";
      }

      client_packet.client_port = client->getRemotePort();

      if (packet_received_callback_) {
        packet_received_callback_(&client_packet);
      }
    }
  }
}

void ServerNetworkManager::SendPacket(sf::Packet* packet,
                                      const Port client_port) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;

  const auto it = std::find_if(
      clients_.begin(), clients_.end(),
      [client_port](const std::unique_ptr<sf::TcpSocket>& client) {
        if (client == nullptr) return false;
        return client->getRemotePort() == client_port;
    }
  );

  if (it == clients_.end()) {
    std::cerr << "There is no client on port : " << client_port << "\n";
  }

  auto& client = *it;

  do {
    status = client->send(*packet);
  } while (status == sf::Socket::Partial);

  if (status == sf::Socket::Disconnected) {
    RemoveClient(client);
    return;
  }

  if (status != sf::Socket::Done) {
    std::cerr << "Could not send packet to client.\n";
  }
}