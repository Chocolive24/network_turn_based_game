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
      [this](const Port client_port) {
        OnClientDisconnection(client_port);
    }
  );

  http_interface_ = http_interface;
}

void Server::Run() noexcept {
  http_interface_->RegisterHostAndPort("127.0.0.1", 8000);

  lobbies_.resize(kStartLobbyCount, Lobby());

  while (true) {
    server_network_interface_->PollEvents();
  }
}

bool Server::UpdatePlayerElo(std::string username, int elo_gain) {
  std::string uri = "/player/" + username;

  const std::string json_body = R"({"gain": )" + std::to_string(elo_gain) + "}";

  http_interface_->Post(uri, json_body);

  uri += "/elo";
  sf::Packet new_elo_packet{};
  auto elo_response = http_interface_->Get(uri);

  // Find the position of the integer field in the JSON response
  const std::size_t int_field_pos = elo_response.find("\"elo\":", 0);
  if (int_field_pos == std::string::npos) {
    // Handle error: integer field not found in response
    std::cerr << "Error: Integer field not found in JSON response.\n";
    return true;
  }

  // Extract the substring containing the integer value
  const std::size_t int_value_start =
    elo_response.find(':', int_field_pos) + 1;
  const std::size_t int_value_end =
    elo_response.find_first_not_of("0123456789", int_value_start);
  const int elo = std::stoi(elo_response.substr(
    int_value_start, int_value_end - int_value_start));

  new_elo_packet << PacketType::kEloUpdated << elo;
  return false;
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
    case PacketType::kJoinLobby: {
      std::string username{};
      client_packet->data >> username;
      AddClientToLobby(client_packet->client_port, username);
      break;
    }
    case PacketType::KStartGame:
      break;
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity:
    case PacketType::kBallStateCorrections:
      for (const auto& lobby : lobbies_) {
        Port other_client_port = 0;

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
        break;
      }
      break;
    case PacketType::kClientIdentification: {
      std::string username{};
      client_packet->data >> username;
      const std::string uri = "/player/" + username;
      auto response = http_interface_->Get(uri);
      if (response.empty()) {
        const std::string json_body = R"({"name": ")" + username + R"(", "elo": 0})";
        http_interface_->Post("/player", json_body);
        response = http_interface_->Get(uri);
      }

      // Find the position of the string field in the JSON response
      const std::size_t string_field_pos = response.find("\"name\":");
      if (string_field_pos == std::string::npos) {
        // Handle error: string field not found in response
        std::cerr << "Error: String field not found in JSON response\n";
        return;
      }

      // Find the position of the integer field in the JSON response
      const std::size_t int_field_pos = response.find("\"elo\":", string_field_pos);
      if (int_field_pos == std::string::npos) {
        // Handle error: integer field not found in response
        std::cerr << "Error: Integer field not found in JSON response.\n";
        return;
      }

      // Find the position of the string value in the JSON response
      const size_t string_value_start = response.find('"', string_field_pos + 
          sizeof("\"name\":") - 1) + 1;
      const std::size_t string_value_end = response.find('"', string_value_start);
      const std::string string_value = response.substr(string_value_start, 
          string_value_end - string_value_start);

      // Extract the substring containing the integer value
      const std::size_t int_value_start = response.find(':', int_field_pos) + 1;
      const std::size_t int_value_end = response.find_first_not_of("0123456789", 
          int_value_start);
      const int int_value = std::stoi(response.substr(int_value_start, 
          int_value_end - int_value_start));

      sf::Packet user_data_packet{};
      user_data_packet << PacketType::kClientIdentification << string_value
                       << int_value;
      server_network_interface_->SendPacket(&user_data_packet,
                                            client_packet->client_port);
      break;
    }
    case PacketType::kGameWon: {
      std::string winner_username{};
      client_packet->data >> winner_username;
      if (!UpdatePlayerElo(winner_username, 100)) {
        std::cerr << "Cannot update elo of " << winner_username << '\n';
      }
      for (auto& lobby : lobbies_) {
        Port other_client_port = 0;
        std::string other_username{};

        if (client_packet->client_port == lobby.client_1_port) {
          other_client_port = lobby.client_2_port;
          other_username = lobby.username_2;
        }
        else if (client_packet->client_port == lobby.client_2_port) {
          other_client_port = lobby.client_1_port;
          other_username = lobby.username_1;
        }

        if (other_client_port == 0) {
          continue;
        }

        if (!UpdatePlayerElo(other_username, -100)) {
          std::cerr << "Cannot update elo of " << other_username << '\n';
        }

        sf::Packet lose_packet{};
        lose_packet << PacketType::kGameLost;
        server_network_interface_->SendPacket(&lose_packet, other_client_port);

        lobby.Clear();

        break;
      }

      break;
    }
    case PacketType::kGameLost:
      break;
    default:
      break;
  }
}

void Server::OnClientDisconnection(const Port client_port) noexcept {
  for (auto& lobby : lobbies_) {
    Port other_client_id = 0;

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

void Server::AddClientToLobby(Port client_port,
                              std::string_view username) noexcept {
  const auto lobby_it =
      std::find_if(lobbies_.begin(), lobbies_.end(),
                   [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddClient(client_port, username);

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