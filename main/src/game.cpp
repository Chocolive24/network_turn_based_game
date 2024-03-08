#include "game.h"

// TODO: socket non blocking + envoyer les packets.

void Game::Run() noexcept {
  if (Init() == ReturnStatus::kFailure) {
    return;
  }

  Update();

  Deinit();
}

ReturnStatus Game::Init() noexcept {
  timer_.Init();
  world_.Init(Math::Vec2F::Zero(), kBallCount_);
  world_.SetContactListener(this);

  if (client_.ConnectToServer(HOST_NAME, PORT) == 
     ReturnStatus::kFailure) {
    return ReturnStatus::kFailure;
  }

  client_.SetBlocking(false);

  CalculateStartBallPositions();

  for (int i = 0; i < kBallCount_; i++) {
    body_refs_[i] = world_.CreateBody();
    auto& body = world_.GetBody(body_refs_[i]);
    body.SetBodyType(PhysicsEngine::BodyType::Dynamic);
    const float f_i = static_cast<float>(i) * 0.5f;
    std::cout << start_ball_pos_[i].X << " " << start_ball_pos_[i].Y << '\n';
    body.SetPosition(start_ball_pos_[i]);
    body.SetDamping(0.75f);

    collider_refs_[i] = world_.CreateCollider(body_refs_[i]);
    auto& collider = world_.GetCollider(collider_refs_[i]);
    collider.SetShape(Math::CircleF(Math::Vec2F::Zero(), kMeterRadius_));
    collider.SetRestitution(0.75f);
  }

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), "Pool",
                sf::Style::Default, window_settings);
  window_.setVerticalSyncEnabled(true);

  return ReturnStatus::kSuccess;
}

void Game::HandleWindowEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
    case sf::Event::Closed:
      window_.close();
      break;
    case sf::Event::MouseButtonPressed:
      is_mouse_pressed_ = true;
      is_mouse_released_ = false;
      break;
    case sf::Event::MouseButtonReleased:
      is_mouse_released_ = true;
      is_mouse_pressed_ = false;
    default:
      break;
    }
  }
}

void Game::Update() noexcept {
  auto& cue_ball_body = world_.GetBody(body_refs_[0]);

  while (window_.isOpen()) {
    HandleWindowEvents();

    CheckForReceivedPackets();

    bool is_mouse_just_pressed =
        was_mouse_pressed_ == false && is_mouse_pressed_ == true;

    timer_.Tick();

    if (is_player_turn_) {
      const auto mouse_pos = Math::Vec2F(sf::Mouse::getPosition(window_).x,
                                         sf::Mouse::getPosition(window_).y);
      const auto ball_pos_in_pix = Metrics::MetersToPixels(
          Math::Vec2F(cue_ball_body.Position().X, cue_ball_body.Position().Y));
      const auto distance = mouse_pos - ball_pos_in_pix;

      if (distance.Length<float>() <= kPixelRadius_ && is_mouse_just_pressed) {
        std::cout << "mouse on circle\n";
        is_charging = true;
      }

      if (is_charging && is_mouse_released_) {
        is_charging = false;

        force_applied_to_ball_ = -Metrics::PixelsToMeters(distance);

        cue_ball_body.SetVelocity(Math::Vec2F(force_applied_to_ball_));
        sf::Packet force_applied_packet;
        force_applied_packet << PacketType::kForceAppliedToBall
                             << force_applied_to_ball_.X
                             << force_applied_to_ball_.Y;
        client_.SendPacket(force_applied_packet);
      }
    }

    std::cout << force_applied_to_ball_.X << " " << force_applied_to_ball_.Y
              << '\n';

    world_.Update(timer_.DeltaTime());

    was_mouse_pressed_ = is_mouse_pressed_;

    // Send current position to server.
    //Packet packet(PacketType::kForceAppliedToBall);
    //sf::Vector2i sf_pos(cue_ball_body.Position().X, cue_ball_body.Position().Y);
    //if (!(packet << PacketType::kForceAppliedToBall << sf_pos.x << sf_pos.y)) {
    //  std::cerr << "Failed to set data into packet." << '\n';
    //}
    //else {
    //  client_.SendPacket(packet);
    //}

    Draw();
  }
}

void Game::Draw() noexcept {
  // Clear window.
  window_.clear(sf::Color::Black);

  // Draw balls.
  // -----------
  for (std::int16_t i = 0; i < kBallCount_; i++) {
    sf::CircleShape circle(kPixelRadius_);
    circle.setOrigin(kPixelRadius_, kPixelRadius_);

    auto& ball_body = world_.GetBody(body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(ball_body.Position());
    circle.setPosition(pixel_pos.X, pixel_pos.Y);

    auto color = i == 0 ? sf::Color::White : sf::Color::Red;
    circle.setFillColor(color);

    window_.draw(circle);
  }

  // Display window.
  window_.display();
}

void Game::Deinit() noexcept { world_.Deinit(); }

void Game::CalculateStartBallPositions() noexcept {
  constexpr Math::Vec2F window_size_meters =
      Metrics::PixelsToMeters(Math::Vec2F(kWindowWidth_, kWindowHeight_));

  start_ball_pos_[0] =
      Math::Vec2F(window_size_meters.X * 0.5f, window_size_meters.Y * 0.66f);

  int maxHeight = 5;
  float height = 0;
  Math::Vec2F startPos = Math::Vec2F(window_size_meters.X * 0.5f, window_size_meters.Y * 0.4f);

  int index = 1;

  static constexpr float space = kMeterRadius_ * 2.f;

  for (int i = 1; i <= maxHeight; i++) {
    auto startPosition =
        startPos + Math::Vec2F(-((i - 1) * space) / 2.f, height);

    for (int nbBall = 0; nbBall < i; nbBall++) {
      start_ball_pos_[index] = (startPosition + Math::Vec2F(nbBall * space, 0));
      index++;
    }

    height += space * sqrt(3.0) / 2.0;
  }
}

void Game::CheckForReceivedPackets() noexcept {
  sf::Packet received_packet;
  auto& cue_ball_body = world_.GetBody(body_refs_[0]);
  switch (client_.ReceivePacket(received_packet)) {
    case PacketType::kNone:
      std::cerr << "Packed received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kForceAppliedToBall:
      received_packet >> force_applied_to_ball_.X >> force_applied_to_ball_.Y;
      cue_ball_body.SetVelocity(force_applied_to_ball_);
      break;
    case PacketType::KStartGame:
      received_packet >> is_player_turn_;
      std::cout << "YOUR TURN\n";
      break;
  }
}
