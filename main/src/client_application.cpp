#include "client_application.h"

ClientApplication::ClientApplication(NetworkInterface* client,
                                     GraphicsInterface* graphics_manager) noexcept :
  client_(client),
  game_graphics_manager(graphics_manager)
{}

ReturnStatus ClientApplication::Run() noexcept {
  Init();

  LaunchLoop();

  Deinit();

  return ReturnStatus::kSuccess;
}

void ClientApplication::Init() noexcept {
  game_manager_.InitGame(client_, Math::Vec2F(kWindowWidth_, kWindowHeight_));

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), "8-Ball Pool",
                 sf::Style::Close, window_settings);
  window_.setFramerateLimit(kFrameRateLimit);

  game_manager_.Init(&window_);
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

    game_manager_.OnEvent(event);
  }
}

//TODO: faire un game_manager.OnPacketReceived(Packet type).
// TODO: ClientApp::CheckForReceivedPackets() -> switch (client->receivepacket) si packet de type lobby ou autre, fait action
//TODO: si packet de type game, le donne au jeu -> game_manager.OnPacketReceived, comme pour handle events.

//void ClientApplication::CheckForReceivedPackets() noexcept {
//  sf::Packet received_packet;
//
//  switch (client_->ReceivePacket(received_packet)) {
//    case PacketType::kNone:
//      std::cerr << "Packed received has no type. \n";
//      break;
//    case PacketType::KNotReady:
//      break;
//    case PacketType::KCueBallVelocity: {
//      received_packet >> game_manager_.force_applied_to_ball_.X >>
//          game_manager_.force_applied_to_ball_.Y;
//      std::cout << std::setprecision(200) << "Received "
//                << game_manager_.force_applied_to_ball_.X << " "
//                << game_manager_.force_applied_to_ball_.Y << '\n';
//      const auto cue_ball_b_ref =
//          game_manager_.world_.GetCollider(game_manager_.ball_collider_refs_[0])
//              .GetBodyRef();
//      auto& cue_ball_body = game_manager_.world_.GetBody(cue_ball_b_ref);
//      cue_ball_body.SetVelocity(game_manager_.force_applied_to_ball_);
//      break;
//    }
//    case PacketType::KStartGame: {
//      received_packet >> game_manager_.has_game_started >>
//          game_manager_.player_index_;
//      break;
//    }
//    case PacketType::kNewTurn: {
//      received_packet >> game_manager_.is_player_turn_;
//      const auto cue_ball_b_ref =
//          game_manager_.world_.GetCollider(game_manager_.ball_collider_refs_[0])
//              .GetBodyRef();
//      auto& cue_ball_body = game_manager_.world_.GetBody(cue_ball_b_ref);
//      cue_ball_body.SetVelocity(Math::Vec2F::Zero());
//      break;
//    }
//    case PacketType::kBallStateCorrections: {
//      for (const auto& col_ref : game_manager_.ball_collider_refs_) {
//        const auto& body_ref =
//            game_manager_.world_.GetCollider(col_ref).GetBodyRef();
//        auto& body = game_manager_.world_.GetBody(body_ref);
//        Math::Vec2F ball_pos = Math::Vec2F::Zero();
//        bool enabled = false;
//        received_packet >> ball_pos.X >> ball_pos.Y >> enabled;
//        body.SetPosition(ball_pos);
//        game_manager_.world_.GetCollider(col_ref).SetEnabled(enabled);
//      }
//      break;
//    }
//    default:
//      break;
//  }
//}

void ClientApplication::LaunchLoop() noexcept {
  while (window_.isOpen()) {
    HandleWindowEvents();

    //CheckForReceivedPackets();

    const auto mouse_pos =
        static_cast<sf::Vector2f>(sf::Mouse::getPosition(window_));
    game_manager_.Update(Math::Vec2F(mouse_pos.x, mouse_pos.y));

    window_.clear(sf::Color::Black);
    game_manager_.Draw();
    window_.display();
  }
}

void ClientApplication::Deinit() noexcept {
  game_manager_.Deinit();
}