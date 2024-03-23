#include "server.h"

#include <iostream>

Server::Server(ServerNetworkInterface* server_net_interface) noexcept {
  server_network_interface_ = server_net_interface;
  server_network_interface_->RegisterPacketReceivedCallback(
      [this](ClientPacket* client_packet) {
        OnPacketReceived(client_packet);
    }
  );
  server_network_interface_->RegisterClientDisconnectionCallback(
      [this](ClientPort client_port) {
        OnClientDisconnect(client_port);
    }
  );
}

void Server::Run() noexcept {
  lobbies_.resize(kStartLobbyCount, Lobby());

  while (true) {
    server_network_interface_->PollEvents();
  }
}

void Server::AddClientToLobby(ClientPort client_id) noexcept {
  const auto lobby_it =
      std::find_if(lobbies_.begin(), lobbies_.end(),
                   [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddClient(client_id);

  sf::Packet joined_lobby_packet{};
  joined_lobby_packet << PacketType::kJoinLobby;
  server_network_interface_->SendPacket(&joined_lobby_packet, client_id);

  if (lobby_it->IsComplete()) {
    sf::Packet p1_start_packet;
    p1_start_packet << PacketType::KStartGame << true << 0;
    server_network_interface_->SendPacket(&p1_start_packet,
                                          lobby_it->client_1_port);
    sf::Packet p2_start_packet;
    p2_start_packet << PacketType::KStartGame << true << 1;
    server_network_interface_->SendPacket(&p2_start_packet,
                                          lobby_it->client_2_port);

    sf::Packet new_turn_packet;
    new_turn_packet << PacketType::kNewTurn << true;
    server_network_interface_->SendPacket(&new_turn_packet,
                                          lobby_it->client_1_port);
  }
}

void Server::OnPacketReceived(ClientPacket* client_packet) noexcept {
  PacketType packet_type = PacketType::kNone;
  client_packet->data >> packet_type;

  switch (packet_type) {
    case PacketType::kNone:
      std::cerr << "Packet received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kJoinLobby:
      std::cout << "LOBBBYYYYYY\n";
      AddClientToLobby(client_packet->client_port);
      break;
    case PacketType::KStartGame:
      break;
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity:
    case PacketType::kBallStateCorrections:
      for (const auto& lobby : lobbies_) {
        ClientPort other_client_port = 0;

        if (client_packet->client_port == lobby.client_1_port) {
          other_client_port = lobby.client_2_port;
        }
        else if (client_packet->client_port == lobby.client_2_port) {
          other_client_port = lobby.client_1_port;
        }

        if (other_client_port == 0) {
          continue;
        }

        server_network_interface_->SendPacket(&client_packet->data,
                                              other_client_port);
      }
      break;
    default:
      break;
  }
}

void Server::OnClientDisconnect(const ClientPort client_port) noexcept {
  for (auto& lobby : lobbies_) {
    ClientPort other_client_id = 0;

    if (client_port == lobby.client_1_port) {
      other_client_id = lobby.client_2_port;
    }
    else if (client_port == lobby.client_2_port) {
      other_client_id = lobby.client_1_port;
    }

    if (other_client_id == 0) {
      continue;
    }

    sf::Packet win_packet{};
    win_packet << PacketType::kGameWon;
    server_network_interface_->SendPacket(&win_packet, other_client_id);

    lobby.Clear();
  }
}
