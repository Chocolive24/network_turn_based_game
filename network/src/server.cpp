#include "server.h"

#include <iostream>

//ReturnStatus Server::Init(const unsigned short port) noexcept {
//  //if (listener_.listen(port) != sf::Socket::Done) {
//  //  std::cerr << "Could not listen to port : " << port << '.' << '\n';
//  //  return ReturnStatus::kFailure;
//  //}
//
//  //listener_.setBlocking(false);
//
//  //std::cout << "Server is listening to port : " << port << '\n';
//
//  //socket_selector_.add(listener_);
//
//
//  return ReturnStatus::kSuccess;
//}

Server::Server(ServerNetworkInterface* server_net_interface) noexcept :
  server_network_interface_(server_net_interface)
{}

void Server::Run() noexcept {
  lobbies_.resize(kStartLobbyCount);

  while (true) {
    if (server_network_interface_->WaitForNetworkEvent(5.f))
    {
      if (server_network_interface_->AcceptNewConnection()) {
        const auto lobby_it = std::find_if(
            lobbies_.begin(), lobbies_.end(),
            [](const Lobby& lobby) { return !lobby.IsComplete(); });

        lobby_it->AddPlayer(client_count_);
        client_count_++;

        if (lobby_it->IsComplete()) {
          std::cout << "Lobby complete !\n";

          sf::Packet p1_start_packet;
          p1_start_packet << PacketType::KStartGame << true << 0;
          server_network_interface_->SendPacket(&p1_start_packet,
                                                lobby_it->client_1_id);
          sf::Packet p2_start_packet;
          p2_start_packet << PacketType::KStartGame << true << 1;
          server_network_interface_->SendPacket(&p2_start_packet,
                                                lobby_it->client_2_id);

          sf::Packet new_turn_packet;
          new_turn_packet << PacketType::kNewTurn << true;
          server_network_interface_->SendPacket(&new_turn_packet,
                                                lobby_it->client_1_id);
        }
      }
      else
      {
        for (ClientId id = 0; id < client_count_; id++) {
          sf::Packet received_packet{};
          switch (server_network_interface_->ReceivePacket(&received_packet, id)) {
            case PacketType::kNone:
              std::cerr << "Packet received has no type. \n";
              break;
            case PacketType::KNotReady:
              break;
            case PacketType::kJoinLobby:
              break;
            case PacketType::KStartGame:
              break;
            case PacketType::kNewTurn:
            case PacketType::KCueBallVelocity:
            case PacketType::kBallStateCorrections:
              for (const auto& lobby : lobbies_) {
                ClientId other_client_id = -1;

                if (id == lobby.client_1_id) {
                  other_client_id = lobby.client_2_id;
                } else if (id == lobby.client_2_id) {
                  other_client_id = lobby.client_1_id;
                }

                if (static_cast<int>(other_client_id) == -1) {
                  continue;
                }

                server_network_interface_->SendPacket(&received_packet,
                                                      other_client_id);
              }
              break;
            default:
              break;
          }
        }
      }
    }
    else {
      // Handle other server tasks or timeout operations here.
      std::cout << "Waiting for activity...\n";
    }

    //// Make the selector wait for data on any socket.
    //if (socket_selector_.wait(sf::seconds(5.f))) {
    //  // Test the listener
    //  if (socket_selector_.isReady(listener_)) {
    //    // The listener is ready: there is a pending connection.
    //    AcceptClient();
    //  }
    //  else {
    //    // Check for data from clients.
    //    CommunicatePacketBetweenClients();
    //  } 
    //} // if selector.wait()
    //else {
    //  // Handle other server tasks or timeout operations here.
    //  std::cout << "Waiting for activity...\n";
    //}
  }
}

//void Server::AcceptClient() noexcept {
//  clients_.emplace_back(std::make_unique<sf::TcpSocket>());
//  const auto& client = clients_.back();
//  if (listener_.accept(*client) != sf::Socket::Done) {
//    std::cerr << "Could not accept client.\n";
//  }
//
//  client->setBlocking(false);
//  socket_selector_.add(*client);
//
//  std::cout << "Client " << client->getRemoteAddress() << ':'
//            << client->getRemotePort() << " is connected. \n";
//
//  const auto lobby_it = std::find_if(lobbies_.begin(), lobbies_.end(),
//               [](const Lobby& lobby) { return !lobby.IsComplete(); });
//
//  lobby_it->AddPlayer(clients_.size() - 1);
//
//  if (lobby_it->IsComplete())
//  {
//    std::cout << "Lobby complete !\n";
//
//    sf::Packet p1_start_packet;
//    p1_start_packet << PacketType::KStartGame << true << 0;
//    if (clients_[lobby_it->client_1_id]->send(p1_start_packet) != sf::Socket::Done) {
//      std::cerr << "Could not send start game packet to player1.\n";
//    }
//    sf::Packet p2_start_packet;
//    p2_start_packet << PacketType::KStartGame << true << 1;
//    if (clients_[lobby_it->client_2_id]->send(p2_start_packet) != sf::Socket::Done) {
//      std::cerr << "Could not send start game packet to player2.\n";
//    }
//
//    sf::Packet new_turn_packet;
//    new_turn_packet << PacketType::kNewTurn << true;
//    if (clients_[lobby_it->client_1_id]->send(new_turn_packet) != sf::Socket::Done) {
//      std::cerr << "Could not send new turn packet to player1.\n";
//    }
//  }
//}
//
//void Server::CommunicatePacketBetweenClients() noexcept {
//  // Iterates throw all clients to receive their potential packets.
//  // --------------------------------------------------------------
//  for (std::size_t i = 0; i < clients_.size(); i++) {
//    const auto client = clients_[i].get();
//
//    if (socket_selector_.isReady(*client)) {
//      sf::Packet packet_received;
//      sf::Socket::Status status = sf::Socket::Partial;
//      do {
//        status = client->receive(packet_received);
//      } while (status == sf::Socket::Partial);
//
//      if (status != sf::Socket::Done) {
//        std::cerr << "Could not receive packet from client.\n";
//      }
//
//      // Iterates throw all lobbies to find the client's opponent.
//      // ---------------------------------------------------------
//      for (const auto& lobby : lobbies_) {
//        sf::TcpSocket* other_client = nullptr;
//        
//        if (i == lobby.client_1_id) {
//          other_client = clients_[lobby.client_2_id].get();
//        }
//        else if (i == lobby.client_2_id) {
//          other_client = clients_[lobby.client_1_id].get();
//        }
//
//        if (other_client == nullptr) {
//          continue;
//        }
//
//        status = sf::Socket::Partial;
//        do {
//          status = other_client->send(packet_received);
//        } while (status == sf::Socket::Partial);
//
//        if (status != sf::Socket::Done) {
//          std::cerr << "Could not send data to other client.\n";
//        }
//      }
//    }
//  }
//}