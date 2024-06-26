#include "client_application.h"
#include "main_menu_gui.h"
#include "lobby_gui.h"

#include <iostream>

#include "identification_gui.h"

ClientApplication::ClientApplication(ClientNetworkInterface* client_net_interface) noexcept :
  client_network_interface_(client_net_interface)
{}

ReturnStatus ClientApplication::Run() noexcept {
  Init();   

  LaunchLoop();

  Deinit();

  return ReturnStatus::kSuccess;
}

void ClientApplication::SendPacket(sf::Packet* packet) const noexcept {
  client_network_interface_->SendPacket(*packet);
}

void ClientApplication::Init() noexcept {
  client_identifier_.RegisterIdentificationCallback(
    [this](const std::string_view username) {
      OnClientIdentified(username);
    } 
  );

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), "8-Ball Pool",
                 sf::Style::Close, window_settings);
  window_.setVerticalSyncEnabled(true);

  font_.loadFromFile("data/Payback.otf");

  current_gui_ = std::make_unique<IdentificationGui>();
}

void ClientApplication::PollWindowEvents() noexcept {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window_.close();
        break;
      case sf::Event::TextEntered: {
        if (state_ == ClientAppState::kUserIdentification) {
          client_identifier_.OnTextEntered(event.text);
        }
        break;
      }
      default:
        break;
    }

    if (state_ == ClientAppState::kInGame) {
      game_.OnEvent(event);
    }

    if (current_gui_) {
      current_gui_->OnEvent(event);
    }
  }
}

void ClientApplication::PollNetworkEvents() noexcept {
  sf::Packet received_packet;
  const auto packet_type =
      client_network_interface_->ReceivePacket(received_packet);
  switch (packet_type) {
    case PacketType::kNone:
      std::cerr << "Packet received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kJoinLobby:
      current_gui_ = std::make_unique<LobbyGui>();
      break;
    case PacketType::kClientIdentification:
      received_packet >> player_data_.username;
      received_packet >> player_data_.elo;
      state_ = ClientAppState::kInMainMenu;
      current_gui_ = std::make_unique<MainMenuGui>(this);
      break;
    case PacketType::kEloUpdated:
      received_packet >> player_data_.elo;
      break;
    case PacketType::KStartGame:
      state_ = ClientAppState::kInGame;
      current_gui_.reset();
      game_.InitGame(client_network_interface_, &window_,
                     Math::Vec2F(kWindowWidth_, kWindowHeight_),
                     player_data_.username);
      game_.client_app = this;
      break;
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity:
    case PacketType::kEndGame:
      break;
    default:
      break;
  }

  if (state_ == ClientAppState::kInGame) {
    game_.OnPacketReceived(&received_packet, packet_type);
  }
}

void ClientApplication::LaunchLoop() noexcept {
  while (window_.isOpen()) {
    PollWindowEvents();

    const auto mouse_pos =
        static_cast<sf::Vector2f>(sf::Mouse::getPosition(window_));

    PollNetworkEvents();

    window_.clear(sf::Color::Black);

    switch (state_) {
      case ClientAppState::kUserIdentification: {
        /*if (current_gui_) {
          current_gui_.reset();
        }*/
        const std::string name_str = client_identifier_.username().data();
        sf::Text username_prompt("username: " + name_str, font_);
        username_prompt.setCharacterSize(30);
        const auto bounds = username_prompt.getGlobalBounds();
        username_prompt.setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
        username_prompt.setPosition(kWindowWidth_ * 0.5f,
                                    kWindowHeight_ * 0.5f);
        window_.draw(username_prompt);
        break;
      }
      case ClientAppState::kInMainMenu:
        if (!current_gui_) {
          current_gui_ = std::make_unique<MainMenuGui>(this);
        }
        break;
      case ClientAppState::kInLobby:
        break;
      case ClientAppState::kInGame: {
        game_.Update(Math::Vec2F(mouse_pos.x, mouse_pos.y));
        game_.Draw();
        break;
      }
    }

    if (current_gui_ != nullptr) {
      current_gui_->Update(mouse_pos);
      current_gui_->Draw(&window_);
    }

    window_.display();
  }
}

void ClientApplication::OnClientIdentified(const std::string_view username) const noexcept {
  sf::Packet username_packet{};
  username_packet << PacketType::kClientIdentification << username.data();
  client_network_interface_->SendPacket(username_packet);
}

void ClientApplication::Deinit() noexcept {
  game_.Deinit();
}
