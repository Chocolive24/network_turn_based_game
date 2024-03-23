#include "server.h"

#include <iostream>

Server::Server(ServerNetworkInterface* server_net_interface) noexcept :
  server_network_interface_(server_net_interface)
{}

void Server::Run() noexcept {
  lobbies_.resize(kStartLobbyCount);

  while (true) {
    if (server_network_interface_->WaitForNetworkEvent(5.f))
    {
      if (server_network_interface_->AcceptNewConnection()) {
        //AddClientToLobby();
        client_count_++;
      }
      else
      {
        HandleReceivedPackets();
      }
    }
    else {
      // Handle other server tasks or timeout operations here.
      std::cout << "Waiting for activity...\n";
    }
  }
}

void Server::AddClientToLobby() noexcept {
  const auto lobby_it =
      std::find_if(lobbies_.begin(), lobbies_.end(),
                   [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddPlayer(client_count_ - 1);

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

void Server::HandleReceivedPackets() noexcept {
  for (ClientId id = 0; id < client_count_; id++) {
    sf::Packet received_packet{};
    switch (server_network_interface_->ReceivePackets(&received_packet, id)) {
      case PacketType::kNone:
        std::cerr << "Packet received has no type. \n";
        break;
      case PacketType::KNotReady:
        break;
      case PacketType::kJoinLobby:
        AddClientToLobby();
        server_network_interface_->SendPacket(&received_packet, id);
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