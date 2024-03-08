#include "game.h"

void Game::Run() noexcept {
  if (Init() == ReturnStatus::kFailure) {
    return;
  }

  Update();

  Deinit();
}

void Game::CreateWalls() {
  constexpr auto win_size_meter = Metrics::PixelsToMeters(
    Math::Vec2F(kWindowWidth_, kWindowHeight_));

  wall_body_refs_[0] = world_.CreateBody();
  auto& body_0 = world_.GetBody(wall_body_refs_[0]);
  body_0.SetBodyType(PhysicsEngine::BodyType::Static);
  body_0.SetPosition(
    Math::Vec2F(win_size_meter.X - 0.5f, win_size_meter.Y * 0.5f));

  wall_col_refs_[0] = world_.CreateCollider(wall_body_refs_[0]);
  auto& col_0 = world_.GetCollider(wall_col_refs_[0]);
  auto size = Math::Vec2F(0.5f, -win_size_meter.Y - 0.5f);
  auto half_size = size * 0.5f;
  col_0.SetShape(Math::RectangleF(Math::Vec2F::Zero() - half_size, 
                                Math::Vec2F::Zero() + half_size));
  col_0.SetRestitution(0.75f);

  wall_body_refs_[1] = world_.CreateBody();
  auto& body_1 = world_.GetBody(wall_body_refs_[1]);
  body_1.SetBodyType(PhysicsEngine::BodyType::Static);
  body_1.SetPosition(
      Math::Vec2F(0.5f, win_size_meter.Y * 0.5f));

  wall_col_refs_[1] = world_.CreateCollider(wall_body_refs_[1]);
  auto& col_1 = world_.GetCollider(wall_col_refs_[1]);
  size = Math::Vec2F(0.5f, -win_size_meter.Y - 0.5f);
  half_size = size * 0.5f;
  col_1.SetShape(Math::RectangleF(Math::Vec2F::Zero() - half_size,
                                Math::Vec2F::Zero() + half_size));
  col_1.SetRestitution(0.75f);

  wall_body_refs_[2] = world_.CreateBody();
  auto& body_2 = world_.GetBody(wall_body_refs_[2]);
  body_2.SetBodyType(PhysicsEngine::BodyType::Static);
  body_2.SetPosition(Math::Vec2F(win_size_meter.X * 0.5f, -0.5f));

  wall_col_refs_[2] = world_.CreateCollider(wall_body_refs_[2]);
  auto& col_2 = world_.GetCollider(wall_col_refs_[2]);
  size = Math::Vec2F(win_size_meter.X - 1.5f, 0.5f);
  half_size = size * 0.5f;
  col_2.SetShape(Math::RectangleF(Math::Vec2F::Zero() - half_size,
                                  Math::Vec2F::Zero() + half_size));
  col_2.SetRestitution(0.75f);

  wall_body_refs_[3] = world_.CreateBody();
  auto& body_3 = world_.GetBody(wall_body_refs_[3]);
  body_3.SetBodyType(PhysicsEngine::BodyType::Static);
  body_3.SetPosition(Math::Vec2F(win_size_meter.X * 0.5f, win_size_meter.Y + 0.5f));

  wall_col_refs_[3] = world_.CreateCollider(wall_body_refs_[3]);
  auto& col_3 = world_.GetCollider(wall_col_refs_[3]);
  size = Math::Vec2F(win_size_meter.X - 1.5f, 0.5f);
  half_size = size * 0.5f;
  col_3.SetShape(Math::RectangleF(Math::Vec2F::Zero() - half_size,
                                  Math::Vec2F::Zero() + half_size));
  col_3.SetRestitution(0.75f);
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
    ball_body_refs_[i] = world_.CreateBody();
    auto& body = world_.GetBody(ball_body_refs_[i]);
    body.SetBodyType(PhysicsEngine::BodyType::Dynamic);
    const float f_i = static_cast<float>(i) * 0.5f;
    body.SetPosition(start_ball_pos_[i]);
    body.SetDamping(0.01f);

    ball_collider_refs_[i] = world_.CreateCollider(ball_body_refs_[i]);
    auto& collider = world_.GetCollider(ball_collider_refs_[i]);
    collider.SetShape(Math::CircleF(Math::Vec2F::Zero(), kMeterRadius_));
    collider.SetRestitution(0.75f);
  }

  CreateWalls();

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), "Pool",
                sf::Style::Close, window_settings);
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
  while (window_.isOpen()) {
    HandleWindowEvents();

    CheckForReceivedPackets();

    timer_.Tick();

    auto& cue_ball_body = world_.GetBody(ball_body_refs_[0]);

     bool is_mouse_just_pressed =
        was_mouse_pressed_ == false && is_mouse_pressed_ == true;

    if (is_player_turn_ && !has_played_) {
      
      const auto mouse_pos = Math::Vec2F(sf::Mouse::getPosition(window_).x,
                                         sf::Mouse::getPosition(window_).y);
      const auto ball_pos_in_pix = Metrics::MetersToPixels(
          Math::Vec2F(cue_ball_body.Position().X, cue_ball_body.Position().Y));
      const auto distance = mouse_pos - ball_pos_in_pix;
      
      if (distance.Length<float>() <= kPixelRadius_ && is_mouse_just_pressed) {
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

        std::cout << "Sent: " << force_applied_to_ball_.X << " " << force_applied_to_ball_.Y
                  << '\n';

        has_played_ = true;
      }
    }

    world_.Update(kFixedTimeStep);

    Math::Vec2F global_ball_velocities = Math::Vec2F::Zero();
    for (const auto& b_ref : ball_body_refs_) {
      auto& body = world_.GetBody(b_ref);
      global_ball_velocities += body.Velocity();
    }

    if (has_played_ && global_ball_velocities.Length<float>() <= 0.001f) {
      sf::Packet new_turn_packet;
      new_turn_packet << PacketType::kNewTurn << true;
      client_.SendPacket(new_turn_packet);
      is_player_turn_ = false;
      has_played_ = false;
    }

    Draw();

    was_mouse_pressed_ = is_mouse_pressed_;
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

    auto& ball_body = world_.GetBody(ball_body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(ball_body.Position());
    circle.setPosition(pixel_pos.X, pixel_pos.Y);

    sf::Color color;

    if (i == 0) {
      color = sf::Color::White;
    }
    else
    {
      color = i % 2 == 0 ? sf::Color::Blue : sf::Color::Red;
      circle.setFillColor(color);
    }


    window_.draw(circle);
  }

  for (std::int16_t i = 0; i < 4; i++) {
    auto& col = world_.GetCollider(wall_col_refs_[i]);
    auto rect_col = Metrics::MetersToPixels(std::get<Math::RectangleF>(col.Shape()).Size());
    sf::RectangleShape rect(sf::Vector2f(rect_col.X, rect_col.Y));

    rect.setOrigin(rect_col.X * 0.5f, rect_col.Y * 0.5f);

    auto& body = world_.GetBody(wall_body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
    rect.setPosition(pixel_pos.X, pixel_pos.Y);

    wall_color_ = is_player_turn_ ? sf::Color::Green : sf::Color::Magenta;
    rect.setFillColor(wall_color_);

    window_.draw(rect);
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
  auto& cue_ball_body = world_.GetBody(ball_body_refs_[0]);
  switch (client_.ReceivePacket(received_packet)) {
    case PacketType::kNone:
      std::cerr << "Packed received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kForceAppliedToBall:
      received_packet >> force_applied_to_ball_.X >> force_applied_to_ball_.Y;
      std::cout << "Received " << force_applied_to_ball_.X << " "
                << force_applied_to_ball_.Y << '\n';
      cue_ball_body.SetVelocity(force_applied_to_ball_);
      break;
    case PacketType::KStartGame:
      received_packet >> has_game_started;
      break;
    case PacketType::kNewTurn:
      received_packet >> is_player_turn_;
      std::cout << "YOUR TURN\n";
      break;
  }
}
