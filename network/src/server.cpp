#include "server.h"


#include <iostream>

void ServerSocket::SendPacket(sf::Packet& packet) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  do {
    status = socket.send(packet);
  } while (status == sf::Socket::Partial);

  if (status != sf::Socket::Done) {
    std::cerr << "Could not send packet to client.\n";
  }
}

PacketType ServerSocket::ReceivePacket(sf::Packet& packet) noexcept {
  sf::Socket::Status status = sf::Socket::Partial;
  do {
    status = socket.receive(packet);
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

ReturnStatus Server::Init(const unsigned short port) noexcept {
  if (listener_.listen(port) != sf::Socket::Done) {
    std::cerr << "Could not listen to port : " << port << '.' << '\n';
    return ReturnStatus::kFailure;
  }

  listener_.setBlocking(false);

  std::cout << "Server is listening to port : " << port << '\n';

  socket_selector_.add(listener_);

  lobbies_.resize(kStartLobbyCount);

  return ReturnStatus::kSuccess;
}

void Server::Run() noexcept {
  while (true) {
    // Make the selector wait for data on any socket.
    if (socket_selector_.wait(sf::seconds(5.f))) {
      // Test the listener
      if (socket_selector_.isReady(listener_)) {
        // The listener is ready: there is a pending connection.
        // -----------------------------------------------------
        AcceptClient();

        /*if (client_index_ == kMaxClientCount_) {
          sf::Packet p1_start_packet;
          p1_start_packet << PacketType::KStartGame << true << 0;
          if (clients_[0].send(p1_start_packet) != sf::Socket::Done) {
            std::cerr << "Could not send start game packet to player1.\n";
          }
          sf::Packet p2_start_packet;
          p2_start_packet << PacketType::KStartGame << true << 1;
          if (clients_[1].send(p2_start_packet) != sf::Socket::Done) {
            std::cerr << "Could not send start game packet to player2.\n";
          }

          sf::Packet new_turn_packet;
          new_turn_packet << PacketType::kNewTurn << true;
          if (clients_[0].send(new_turn_packet) != sf::Socket::Done) {
            std::cerr << "Could not send new turn packet to player1.\n";
          }
        }*/
      }
      else {
        // Check for data from clients.
        // ----------------------------
        CommunicatePacketBetweenClients();
      } 
    } // if selector.wait()
    else {
      // Handle other server tasks or timeout operations here.
      std::cout << "Waiting for activity...\n";
    }
  }
}

void Server::AcceptClient() noexcept {
  //TODO: client connection part, need to seperate from lobby conecction.
  auto client = std::make_unique<ServerSocket>();

  if (listener_.accept(client->socket) != sf::Socket::Done) {
    std::cerr << "Could not accept client.\n";
  }

  client->socket.setBlocking(false);
  socket_selector_.add(client->socket);

  //clients_[client_index_].setBlocking(false);
  //socket_selector_.add(clients_[client_index_]);

  std::cout << "Client " << client->socket.getRemoteAddress() << ':'
            << client->socket.getRemotePort() << " is connected. \n";
  //client_index_++;

  const auto lobby_it = std::find_if(lobbies_.begin(), lobbies_.end(),
               [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddPlayer(std::move(client));

  if (lobby_it->IsComplete())
  {
    std::cout << "Lobby complete !\n";
  }
}

void Server::CommunicatePacketBetweenClients() noexcept {
  // Iterates throw all clients to receive their potential packets.
  // --------------------------------------------------------------
  for (auto& lobby : lobbies_) {
    if (socket_selector_.isReady(lobby.)) {
      sf::Packet packet_received;
      sf::Socket::Status status = sf::Socket::Partial;
      do
      {
        status = client.receive(packet_received); 
      } while (status == sf::Socket::Partial);

      if (status != sf::Socket::Done) {
        std::cerr << "Could not receive packet from client.\n";
      }

      // Send received data to other clients.
      // ------------------------------------
      for (auto& other_client : clients_) {
        if (&other_client != &client) {
          status = sf::Socket::Partial;
          do {
            status = other_client.send(packet_received);
          } while (status == sf::Socket::Partial);

          if (status != sf::Socket::Done) {
            std::cerr << "Could not send data to other client.\n";
          }
        }
      }
    }  
  }
}