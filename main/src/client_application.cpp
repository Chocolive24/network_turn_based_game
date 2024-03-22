#include "client_application.h"

#include <iostream>

ClientApplication::ClientApplication(ClientNetworkInterface* client) noexcept :
  client_(client)
{}

ReturnStatus ClientApplication::Run() noexcept {
  Init();

  LaunchLoop();

  Deinit();

  return ReturnStatus::kSuccess;
}

void ClientApplication::Init() noexcept {
  game_.InitGame(client_, &window_, Math::Vec2F(kWindowWidth_, kWindowHeight_));

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), "8-Ball Pool",
                 sf::Style::Close, window_settings);
  window_.setFramerateLimit(kFrameRateLimit);
}

void ClientApplication::HandleWindowEvents() {
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
  switch (const auto packet_type = client_->ReceivePacket(received_packet)) {
    case PacketType::kNone:
      std::cerr << "Packet received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kJoinLobby:
    case PacketType::KStartGame:
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity:
    case PacketType::kBallStateCorrections:
      game_.OnPacketReceived(&received_packet, packet_type);
      break;
    default:
      break;
  }
}

void ClientApplication::LaunchLoop() noexcept {
  while (window_.isOpen()) {
    HandleWindowEvents();

    CheckForReceivedPackets();

    const auto mouse_pos =
        static_cast<sf::Vector2f>(sf::Mouse::getPosition(window_));
    game_.Update(Math::Vec2F(mouse_pos.x, mouse_pos.y));

    window_.clear(sf::Color::Black);
    game_.Draw();
    window_.display();
  }
}

void ClientApplication::Deinit() noexcept {
  game_.Deinit();
}