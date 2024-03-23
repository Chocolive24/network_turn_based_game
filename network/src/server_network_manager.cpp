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


void ServerNetworkManager::AcceptNewConnection() noexcept {
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

void ServerNetworkManager::PollClientsPacket() noexcept {
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
        socket_selector_.remove(*client);
        if (disconnect_callback_) disconnect_callback_(client->getRemotePort());
        client.reset();
        continue;
      }

      if (status != sf::Socket::Done) {
        //std::cerr << "Could not receive packet from client.\n";
      }

      client_packet.client_port = client->getRemotePort();

      if (packet_received_callback_) packet_received_callback_(&client_packet);
    }
  }
}

void ServerNetworkManager::SendPacket(sf::Packet* packet,
                                      const ClientPort client_id) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;

  const auto it = std::find_if(
      clients_.begin(), clients_.end(),
      [client_id](const std::unique_ptr<sf::TcpSocket>& client) {
        return client->getRemotePort() == client_id;
    }
  );

  if (it == clients_.end()) {
    std::cerr << "There is no client on port : " << client_id << "\n";
  }

  auto& client = *it;

  do {
    status = client->send(*packet);
  } while (status == sf::Socket::Partial);

  if (status == sf::Socket::Disconnected) {
    socket_selector_.remove(*client);
    if (disconnect_callback_) disconnect_callback_(client->getRemotePort());
    client.reset();
    return;
  }

  if (status != sf::Socket::Done) {
    std::cerr << "Could not send packet to client.\n";
  }
}

void ServerNetworkManager::PollEvents() noexcept {
  if (socket_selector_.wait(sf::seconds(5.f))) {
    if (socket_selector_.isReady(listener_)) {
      AcceptNewConnection();
    }
    else {
      PollClientsPacket();
    }
  }
  else {
    std::cout << "Waiting for activity...\n";
  }
}