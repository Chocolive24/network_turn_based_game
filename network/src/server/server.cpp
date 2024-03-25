#include "server/server.h"

#include <iostream>

Server::Server(ServerNetworkInterface* server_net_interface,
               HttpInterface* http_interface) noexcept {
  server_network_interface_ = server_net_interface;
  server_network_interface_->RegisterPacketReceivedCallback(
      [this](ClientPacket* client_packet) {
        OnPacketReceived(client_packet);
    }
  );
  server_network_interface_->RegisterClientDisconnectionCallback(
      [this](const ClientPort client_port) {
        OnClientDisconnection(client_port);
    }
  );

  http_interface_ = http_interface;
}

void Server::Run() noexcept {
  http_interface_->RegisterHostAndPort("127.0.0.1", 8000);

  constexpr std::string_view body = R"({"name": "Remy"})";
  http_interface_->Post("/players", sf::Http::Request::Post, body);

  const auto& response = http_interface_->Get("/players", 
      sf::Http::Request::Get);
  std::cout << response << '\n';

  lobbies_.resize(kStartLobbyCount, Lobby());

  while (true) {
    server_network_interface_->PollEvents();
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

void Server::OnClientDisconnection(const ClientPort client_port) noexcept {
  for (auto& lobby : lobbies_) {
    ClientPort other_client_id = 0;

    if (client_port == lobby.client_1_port) {
      other_client_id = lobby.client_2_port;
    } else if (client_port == lobby.client_2_port) {
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

void Server::AddClientToLobby(ClientPort client_port) noexcept {
  const auto lobby_it =
      std::find_if(lobbies_.begin(), lobbies_.end(),
                   [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddClient(client_port);

  sf::Packet joined_lobby_packet{};
  joined_lobby_packet << PacketType::kJoinLobby;
  server_network_interface_->SendPacket(&joined_lobby_packet, client_port);

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