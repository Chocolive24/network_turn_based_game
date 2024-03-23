#include "client_application.h"

#include <iostream>

ClientApplication::ClientApplication(ClientNetworkInterface* client_net_interface) noexcept :
  client_network_interface_(client_net_interface)
{}

ReturnStatus ClientApplication::Run() noexcept {
  Init();

  LaunchLoop();

  Deinit();

  return ReturnStatus::kSuccess;
}

void ClientApplication::Init() noexcept {
  game_.InitGame(client_network_interface_, &window_, Math::Vec2F(kWindowWidth_, kWindowHeight_));

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), "8-Ball Pool",
                 sf::Style::Close, window_settings);
  window_.setVerticalSyncEnabled(true);
}

void ClientApplication::PollWindowEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed:
        window_.close();
        break;
      default:
        break;
    }

    game_.OnEvent(event);
  }
}

void ClientApplication::CheckForReceivedPackets() noexcept {
  sf::Packet received_packet;
  switch (const auto packet_type = client_network_interface_->ReceivePacket(received_packet)) {
    case PacketType::kNone:
      std::cerr << "Packet received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kJoinLobby:
      state_ = ClientAppState::kInLobby;
      break;
    case PacketType::KStartGame:
      state_ = ClientAppState::kInGame;
      game_.OnPacketReceived(&received_packet, packet_type);
      break;
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity:
    case PacketType::kBallStateCorrections:
    case PacketType::kGameWon:
    case PacketType::kGameLost:
      game_.OnPacketReceived(&received_packet, packet_type);
      break;
    default:
      break;
  }
}

void ClientApplication::LaunchLoop() noexcept {
  while (window_.isOpen()) {
    PollWindowEvents();

    const auto mouse_pos =
        static_cast<sf::Vector2f>(sf::Mouse::getPosition(window_));

    CheckForReceivedPackets();

    window_.clear(sf::Color::Black);

    switch (state_) {
      case ClientAppState::KEntryPoint: {
        sf::RectangleShape play_button(
            sf::Vector2f(0.5f * kWindowWidth_, 0.1f * kWindowHeight_));
        play_button.setOrigin(play_button.getSize() * 0.5f);
        play_button.setPosition(kWindowWidth_ * 0.5f, kWindowHeight_ * 0.33f);

        const bool is_hover = play_button.getGlobalBounds().contains(mouse_pos);

        const auto color = is_hover ? sf::Color::Green : sf::Color::Blue;
        play_button.setFillColor(color);

        if (is_hover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
          sf::Packet join_lobby_packet;
          join_lobby_packet << PacketType::kJoinLobby;
          client_network_interface_->SendPacket(join_lobby_packet);
        }

        window_.draw(play_button);
        break;
      }

      case ClientAppState::kInLobby: {
        sf::RectangleShape play_button(
            sf::Vector2f(0.5f * kWindowWidth_, 0.1f * kWindowHeight_));
        play_button.setOrigin(play_button.getSize() * 0.5f);
        play_button.setPosition(kWindowWidth_ * 0.5f, kWindowHeight_ * 0.33f);

        const bool is_hover = play_button.getGlobalBounds().contains(mouse_pos);

        const auto color = is_hover ? sf::Color::Red : sf::Color::Yellow;
        play_button.setFillColor(color);

        window_.draw(play_button);
        break;
      }

      case ClientAppState::kInGame: {
        game_.Update(Math::Vec2F(mouse_pos.x, mouse_pos.y));

        game_.Draw();
        break;
      }
    }

    window_.display();
  }
}

void ClientApplication::Deinit() noexcept {
  game_.Deinit();
}