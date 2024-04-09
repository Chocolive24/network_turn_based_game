#include "server.h"

#include <iostream>

#include "http_manager.h"

Server::Server(ServerNetworkInterface* server_net_interface,
               HttpInterface* http_interface) noexcept {
  server_network_interface_ = server_net_interface;
  server_network_interface_->RegisterPacketReceivedCallback(
      [this](ClientPacket* client_packet) { OnPacketReceived(client_packet); });

  server_network_interface_->RegisterClientDisconnectionCallback(
      [this](const Port client_port) { OnClientDisconnection(client_port); });

  http_interface_ = http_interface;

  const std::string uri = "/";
  const auto response = http_interface_->Get(uri);

  if (response == HttpManager::kOfflineMessage) {
    is_database_connected_ = false;
  }
}

void Server::Run() noexcept {
  http_interface_->RegisterHostAndPort("127.0.0.1", 8000);

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
    case PacketType::KStartGame:
    case PacketType::kEloUpdated:
      break;

    case PacketType::kJoinLobby: {
      ClientData client_data{"", client_packet->client_port};
      client_packet->data >> client_data.username;
      AddClientToLobby(client_data);
      break;
    }

    case PacketType::kNewTurn: {
      const Port other_client_port =
          FindClientOpponentInLobbies(client_packet->client_port);

      if (other_client_port == 0) {
        break;
      }

      server_network_interface_->SendPacket(&client_packet->data,
                                            other_client_port);
      break;
    }

    case PacketType::KCueBallVelocity: {
      const Port other_client_port =
          FindClientOpponentInLobbies(client_packet->client_port);

      if (other_client_port == 0) {
        break;
      }

      server_network_interface_->SendPacket(&client_packet->data,
                                            other_client_port);
      server_network_interface_->SendPacket(&client_packet->data,
                                            client_packet->client_port);
      break;
    }

    case PacketType::kClientIdentification: {
      ClientData client_data{"", client_packet->client_port};
      client_packet->data >> client_data.username;
      IdentifyClient(client_data);
      break;
    }

    case PacketType::kEndGame: {
      ClientData winner_data{"", client_packet->client_port};

      for (auto& lobby : lobbies_) {
        ClientData other_client_data{};

        if (client_packet->client_port == lobby.client_data_1.port) {
          winner_data.username = lobby.client_data_1.username;
          other_client_data = lobby.client_data_2;
        }
        else if (client_packet->client_port == lobby.client_data_2.port) {
          winner_data.username = lobby.client_data_2.username;
          other_client_data = lobby.client_data_1;
        }

        if (other_client_data.port == 0) {
          continue;
        }

        if (!UpdatePlayerElo(winner_data, 100)) {
          std::cerr << "Cannot update elo of " << winner_data.username << '\n';
        }

        if (!UpdatePlayerElo(other_client_data, -100)) {
          std::cerr << "Cannot update elo of " << other_client_data.username << '\n';
        }

        sf::Packet win_packet{};
        win_packet << PacketType::kEndGame << true;
        server_network_interface_->SendPacket(&win_packet,
                                              client_packet->client_port);

        sf::Packet lose_packet{};
        lose_packet << PacketType::kEndGame << false;
        server_network_interface_->SendPacket(&lose_packet, other_client_data.port);

        lobby.Clear();

        break;
      }

      break;
    }
    default:
      break;
  }
}

void Server::OnClientDisconnection(const Port client_port) noexcept {
  ClientData disconnected_client_data{"", client_port};
  ClientData winner_client_data{};

  for (auto& lobby : lobbies_) {
    if (client_port == lobby.client_data_1.port) {
      disconnected_client_data.username = lobby.client_data_1.username;
      winner_client_data = lobby.client_data_2;
    }
    else if (client_port == lobby.client_data_2.port) {
      disconnected_client_data.username = lobby.client_data_2.username;
      winner_client_data = lobby.client_data_1;
    }

    if (winner_client_data.port == 0) {
      continue;
    }

    sf::Packet win_packet{};
    win_packet << PacketType::kEndGame << true;
    server_network_interface_->SendPacket(&win_packet, winner_client_data.port);

    if (!UpdatePlayerElo(disconnected_client_data, -100)) {
      std::cerr << "Cannot update elo of " << disconnected_client_data.username
                << '\n';
    }

    if (!UpdatePlayerElo(winner_client_data, 100)) {
      std::cerr << "Cannot update elo of " << winner_client_data.username << '\n';
    }

    lobby.Clear();

    return;
  }
}

void Server::AddClientToLobby(const ClientData& client_data) noexcept {
  const auto lobby_it =
      std::find_if(lobbies_.begin(), lobbies_.end(),
                   [](const Lobby& lobby) { return !lobby.IsComplete(); });

  lobby_it->AddClient(client_data);

  sf::Packet joined_lobby_packet{};
  joined_lobby_packet << PacketType::kJoinLobby;
  server_network_interface_->SendPacket(&joined_lobby_packet, client_data.port);

  if (lobby_it->IsComplete()) {
    sf::Packet p1_start_packet;
    p1_start_packet << PacketType::KStartGame << true << 0;
    server_network_interface_->SendPacket(&p1_start_packet,
                                          lobby_it->client_data_1.port);
    sf::Packet p2_start_packet;
    p2_start_packet << PacketType::KStartGame << true << 1;
    server_network_interface_->SendPacket(&p2_start_packet,
                                          lobby_it->client_data_2.port);

    sf::Packet new_turn_packet;
    new_turn_packet << PacketType::kNewTurn << true;
    server_network_interface_->SendPacket(&new_turn_packet,
                                          lobby_it->client_data_1.port);
  }
}

Port Server::FindClientOpponentInLobbies(const Port client_port) const noexcept {
  for (auto& lobby : lobbies_) {
    if (client_port == lobby.client_data_1.port) {
      return lobby.client_data_2.port;
    }

    if (client_port == lobby.client_data_2.port) {
      return lobby.client_data_1.port;
    }
  }

  return 0;
}

bool Server::UpdatePlayerElo(const ClientData& client_data,
                             int elo_gain) const {
  sf::Packet new_elo_packet{};

  if (!is_database_connected_) {
    new_elo_packet << PacketType::kEloUpdated << 0;
    server_network_interface_->SendPacket(&new_elo_packet, client_data.port);
    return false;
  }

  std::string uri = "/player/" + client_data.username;

  const std::string json_body = R"({"gain": )" + std::to_string(elo_gain) + "}";

  http_interface_->Post(uri, json_body);

  uri += "/elo";
  auto elo_response = http_interface_->Get(uri);

  // Find the position of the integer field in the JSON response
  const std::size_t int_field_pos = elo_response.find("\"elo\":", 0);
  if (int_field_pos == std::string::npos) {
    // Handle error: integer field not found in response
    std::cerr << "Error: Integer field not found in JSON response.\n";
    return true;
  }

  // Extract the substring containing the integer value
  const std::size_t int_value_start = elo_response.find(':', int_field_pos) + 1;
  const std::size_t int_value_end =
      elo_response.find_first_not_of("-0123456789", int_value_start);
  const int elo = std::stoi(
      elo_response.substr(int_value_start, int_value_end - int_value_start));

  new_elo_packet << PacketType::kEloUpdated << elo;
  server_network_interface_->SendPacket(&new_elo_packet, client_data.port);

  return true;
}

void Server::IdentifyClient(const ClientData& client_data) const noexcept {
  if (!is_database_connected_) {
    const std::string username = "offline: " + client_data.username;
    sf::Packet user_data_packet{};
    user_data_packet << PacketType::kClientIdentification << username << 0;
    server_network_interface_->SendPacket(&user_data_packet, client_data.port);
    return;
  }

  const std::string uri = "/player/" + client_data.username;
  auto response = http_interface_->Get(uri);

  if (response.empty()) {
    const std::string json_body =
        R"({"name": ")" + client_data.username + R"(", "elo": 1000})";
    http_interface_->Post("/player", json_body);
    response = http_interface_->Get(uri);
  }

  // Find the position of the string field in the JSON response
  const std::size_t string_field_pos = response.find("\"name\":");
  if (string_field_pos == std::string::npos) {
    // Handle error: string field not found in response
    std::cerr << "Error: String field not found in JSON response\n";
  }

  // Find the position of the integer field in the JSON response
  const std::size_t int_field_pos = response.find("\"elo\":", string_field_pos);
  if (int_field_pos == std::string::npos) {
    // Handle error: integer field not found in response
    std::cerr << "Error: Integer field not found in JSON response.\n";
  }

  // Find the position of the string value in the JSON response
  const size_t string_value_start =
      response.find('"', string_field_pos + sizeof("\"name\":") - 1) + 1;
  const std::size_t string_value_end = response.find('"', string_value_start);
  const std::string string_value = response.substr(
      string_value_start, string_value_end - string_value_start);

  // Extract the substring containing the integer value
  const std::size_t int_value_start = response.find(':', int_field_pos) + 1;
  const std::size_t int_value_end =
      response.find_first_not_of("-0123456789", int_value_start);
  const int int_value = std::stoi(
      response.substr(int_value_start, int_value_end - int_value_start));

  sf::Packet user_data_packet{};
  user_data_packet << PacketType::kClientIdentification << string_value
                   << int_value;

  server_network_interface_->SendPacket(&user_data_packet, client_data.port);
}