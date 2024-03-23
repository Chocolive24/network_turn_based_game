#include "server.h"

#include <iostream>

Server::Server(ServerNetworkInterface* server_net_interface) noexcept {
  server_network_interface_ = server_net_interface;
  server_network_interface_->RegisterPacketReceivedCallback(
      [this](ClientPacket* client_packet) { OnPacketReceived(client_packet); });
}

void Server::Run() noexcept {
  lobbies_.resize(kStartLobbyCount);

  while (true) {
    if (server_network_interface_->WaitForNetworkEvent(5.f))
    {
      if (server_network_interface_->AcceptNewConnection()) {
      }
      else
      {
        server_network_interface_->PollClientPackets();
      }
    }
    else {
      // Handle other server tasks or timeout operations here.
      std::cout << "Waiting for activity...\n";
    }
  }
}

void Server::AddClientToLobby(ClientPort client_id) noexcept {
  const auto lobby_it =
      std::find_if(lobbies_.begin(), lobbies_.end(),
                   [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddPlayer(client_id);

  sf::Packet joined_lobby_packet{};
  joined_lobby_packet << PacketType::kJoinLobby;
  server_network_interface_->SendPacket(&joined_lobby_packet, client_id);

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

void Server::OnPacketReceived(ClientPacket* client_packet) noexcept {
  PacketType packet_type = PacketType::kNone;
  client_packet->packet_data >> packet_type;
  std::cout << client_packet->client_id << "'\n";
  switch (packet_type) {
    case PacketType::kNone:
      std::cerr << "Packet received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kJoinLobby:
      AddClientToLobby(client_packet->client_id);
      break;
    case PacketType::KStartGame:
      break;
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity:
    case PacketType::kBallStateCorrections:
      for (const auto& lobby : lobbies_) {
        ClientPort other_client_id = -1;

        if (client_packet->client_id == lobby.client_1_id) {
          other_client_id = lobby.client_2_id;
        }
        else if (client_packet->client_id == lobby.client_2_id) {
          other_client_id = lobby.client_1_id;
        }

        if (static_cast<int>(other_client_id) == -1) {
          continue;
        }

        server_network_interface_->SendPacket(&client_packet->packet_data,
                                              other_client_id);
      }
      break;
    default:
      break;
  }
}