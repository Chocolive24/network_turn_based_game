#include "game.h"

ReturnStatus Game::Run() noexcept {
  if (Init() == ReturnStatus::kFailure) {
    return ReturnStatus::kFailure;
  }

  Update();

  Deinit();

  return ReturnStatus::kSuccess;
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

  CreateBalls();
  CreateWalls();
  CreateHoles();

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
      break;
    default:
      break;
    }
  }
}

void Game::CheckForReceivedPackets() noexcept {
  sf::Packet received_packet;

  switch (client_.ReceivePacket(received_packet)) {
    case PacketType::kNone:
      std::cerr << "Packed received has no type. \n";
      break;
    case PacketType::KNotReady:
      break;
    case PacketType::kForceAppliedToBall: {
      received_packet >> force_applied_to_ball_.X >> force_applied_to_ball_.Y;
      std::cout << "Received " << force_applied_to_ball_.X << " "
                << force_applied_to_ball_.Y << '\n';
      const auto cue_ball_b_ref =
          world_.GetCollider(ball_collider_refs_[0]).GetBodyRef();
      auto& cue_ball_body = world_.GetBody(cue_ball_b_ref);
      cue_ball_body.SetVelocity(force_applied_to_ball_);
      break;
    }
    case PacketType::KStartGame:
      received_packet >> has_game_started >> player_index_;
      break;
    case PacketType::kNewTurn:
      received_packet >> is_player_turn_;
      break;
    case PacketType::kBallPositionsPacket: {
      for (const auto& col_ref : ball_collider_refs_) {
        const auto& body_ref = world_.GetCollider(col_ref).GetBodyRef();
        auto& body = world_.GetBody(body_ref);
        Math::Vec2F ball_pos = Math::Vec2F::Zero();
        received_packet >> ball_pos.X >> ball_pos.Y;
        body.SetPosition(ball_pos);
      }
      break;
    }
  }
}

void Game::HandlePlayerTurn() {
  const auto cue_ball_body_ref =
    world_.GetCollider(ball_collider_refs_[0]).GetBodyRef();
  auto& cue_ball_body = world_.GetBody(cue_ball_body_ref);

  if (is_player_turn_ && !has_played_) {
      
    const auto mouse_pos = Math::Vec2F(sf::Mouse::getPosition(window_).x,
                                       sf::Mouse::getPosition(window_).y);
    const auto ball_pos_in_pix = Metrics::MetersToPixels(
      Math::Vec2F(cue_ball_body.Position().X, cue_ball_body.Position().Y));
    const auto distance = mouse_pos - ball_pos_in_pix;
      
    if (distance.Length<float>() <= kPixelRadius_ && is_mouse_just_pressed_) {
      is_charging = true;
    }

    if (is_charging && is_mouse_released_) {
      is_charging = false;
        
      force_applied_to_ball_ = -Metrics::PixelsToMeters(distance);
      force_applied_to_ball_ *= 3.f;
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
}

void Game::CheckEndTurnCondition() {
  Math::Vec2F global_ball_velocities = Math::Vec2F::Zero();

  for (const auto& col_ref : ball_collider_refs_) {
    const auto& col = world_.GetCollider(col_ref);
    if (!col.Enabled()) continue;

    const auto body_ref = col.GetBodyRef();
    auto& body = world_.GetBody(body_ref);
    global_ball_velocities += body.Velocity();
  }

  if (has_played_ && global_ball_velocities.Length<float>() <= 0.001f) {
    sf::Packet ball_pos_packet;
    ball_pos_packet << PacketType::kBallPositionsPacket;
    for (const auto& col_ref : ball_collider_refs_) {
      const auto& body_ref = world_.GetCollider(col_ref).GetBodyRef();
      auto& body = world_.GetBody(body_ref);

       ball_pos_packet << body.Position().X << body.Position().Y;
    }
    client_.SendPacket(ball_pos_packet);

    sf::Packet new_turn_packet;
    new_turn_packet << PacketType::kNewTurn << true;
    client_.SendPacket(new_turn_packet);
    is_player_turn_ = false;
    has_played_ = false;
  }
}

void Game::Update() noexcept {
  while (window_.isOpen()) {
    HandleWindowEvents();

    CheckForReceivedPackets();

    timer_.Tick();

    is_mouse_just_pressed_ =
        was_mouse_pressed_ == false && is_mouse_pressed_ == true;

    HandlePlayerTurn();

    world_.Update(kFixedTimeStep);

    CheckEndTurnCondition();

    Draw();

    was_mouse_pressed_ = is_mouse_pressed_;
  }
}

void Game::Draw() noexcept {
  // Clear window.
  const sf::Color clear_color = player_index_ == 0 ? 
      sf::Color::Blue : sf::Color::Red;
  window_.clear(clear_color);

  const sf::Vector2f table_size(kWindowWidth_ - 50.f, kWindowHeight_ - 50.f);
  sf::RectangleShape table_rect(table_size);
  table_rect.setPosition(kWindowWidth_ * 0.5f, kWindowHeight_ * 0.5f);
  table_rect.setOrigin(table_size.x * 0.5f, table_size.y * 0.5f);
  table_rect.setFillColor(sf::Color(76, 153, 0));
  window_.draw(table_rect);

  DrawBalls();
  DrawWalls();
  DrawHoles();

  // Display window.
  window_.display();
}

void Game::Deinit() noexcept { world_.Deinit(); }

void Game::CreateBalls() noexcept {
  ball_collider_refs_.reserve(kBallCount_);

  constexpr Math::Vec2F window_size_meters =
      Metrics::PixelsToMeters(Math::Vec2F(kWindowWidth_, kWindowHeight_));

  start_ball_pos_[0] = kCueBallStartPos;

  constexpr int maxHeight = 5;
  float height = 0;
  constexpr Math::Vec2F startPos = Math::Vec2F(window_size_meters.X * 0.5f, window_size_meters.Y * 0.4f);

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

  for (int i = 0; i < kBallCount_; i++) {
    const auto body_ref = world_.CreateBody();
    auto& body = world_.GetBody(body_ref);
    body.SetBodyType(PhysicsEngine::BodyType::Dynamic);
    const float f_i = static_cast<float>(i) * 0.5f;
    body.SetPosition(start_ball_pos_[i]);
    body.SetDamping(0.01f);

    ball_collider_refs_.push_back(world_.CreateCollider(body_ref));
    auto& collider = world_.GetCollider(ball_collider_refs_[i]);
    collider.SetShape(Math::CircleF(Math::Vec2F::Zero(), kMeterRadius_));
    collider.SetRestitution(0.75f);
  }
}

void Game::CreateWalls() {
  constexpr auto win_size_meter =
      Metrics::PixelsToMeters(Math::Vec2F(kWindowWidth_, kWindowHeight_));

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
  body_1.SetPosition(Math::Vec2F(0.5f, win_size_meter.Y * 0.5f));

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
  body_3.SetPosition(
      Math::Vec2F(win_size_meter.X * 0.5f, win_size_meter.Y + 0.5f));

  wall_col_refs_[3] = world_.CreateCollider(wall_body_refs_[3]);
  auto& col_3 = world_.GetCollider(wall_col_refs_[3]);
  size = Math::Vec2F(win_size_meter.X - 1.5f, 0.5f);
  half_size = size * 0.5f;
  col_3.SetShape(Math::RectangleF(Math::Vec2F::Zero() - half_size,
                                  Math::Vec2F::Zero() + half_size));
  col_3.SetRestitution(0.75f);
}

void Game::CreateHoles() noexcept {
  constexpr auto win_size_meter =
      Metrics::PixelsToMeters(Math::Vec2F(kWindowWidth_, kWindowHeight_));

  float x_pos = win_size_meter.X - 0.875f;
  constexpr float start_y_pos = -0.875f;
  float y_pos = start_y_pos;

  for (int i = 0; i < 6; i++) {
    hole_body_refs_[i] = world_.CreateBody();
    auto& body_0 = world_.GetBody(hole_body_refs_[i]);
    body_0.SetBodyType(PhysicsEngine::BodyType::Kinematic);
    body_0.SetPosition(Math::Vec2F(x_pos, y_pos));

    hole_col_refs_[i] = world_.CreateCollider(hole_body_refs_[i]);
    auto& col_0 = world_.GetCollider(hole_col_refs_[i]);
    col_0.SetShape(Math::CircleF(Math::Vec2F::Zero(), 0.3f));
    col_0.SetIsTrigger(true);

    y_pos += win_size_meter.Y * 0.5f - start_y_pos;

    if (i == 2) {
      x_pos = 0.875f;
      y_pos = start_y_pos;
    }
  }
}

void Game::DrawBalls() {
  int i = 0;
  for (const auto& col_ref : ball_collider_refs_) {
    const auto& col = world_.GetCollider(col_ref);
    if (!col.Enabled()) {
      i++;
      continue;
    }

    sf::CircleShape circle(kPixelRadius_);
    circle.setOrigin(kPixelRadius_, kPixelRadius_);

    auto& ball_body = world_.GetBody(col.GetBodyRef());
    const auto pixel_pos = Metrics::MetersToPixels(ball_body.Position());
    circle.setPosition(pixel_pos.X, pixel_pos.Y);

    sf::Color color;

    if (i == 0) {
      color = sf::Color::White;
    } else if (i == 1) {
      color = sf::Color::Yellow;
    } else {
      color = i % 2 == 0 ? sf::Color::Blue : sf::Color::Red;
    }

    circle.setFillColor(color);
    window_.draw(circle);
    i++;
  }
}

void Game::DrawWalls() {
  for (std::int16_t i = 0; i < 4; i++) {
    auto& col = world_.GetCollider(wall_col_refs_[i]);
    auto rect_col =
        Metrics::MetersToPixels(std::get<Math::RectangleF>(col.Shape()).Size());
    sf::RectangleShape rect(sf::Vector2f(rect_col.X, rect_col.Y));

    rect.setOrigin(rect_col.X * 0.5f, rect_col.Y * 0.5f);

    auto& body = world_.GetBody(wall_body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
    rect.setPosition(pixel_pos.X, pixel_pos.Y);

    wall_color_ = is_player_turn_ ? sf::Color::Green : sf::Color::Magenta;
    rect.setFillColor(sf::Color(102, 51, 0));

    window_.draw(rect);
  }
}

void Game::DrawHoles() {
  for (std::int16_t i = 0; i < 6; i++) {
    auto& col = world_.GetCollider(hole_col_refs_[i]);
    auto radius =
        Metrics::MetersToPixels(std::get<Math::CircleF>(col.Shape()).Radius());

    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);

    auto& body = world_.GetBody(hole_body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
    circle.setPosition(pixel_pos.X, pixel_pos.Y);

    circle.setFillColor(sf::Color::Black);
    window_.draw(circle);
  }
}

void Game::OnTriggerEnter(PhysicsEngine::ColliderRef colliderRefA,
                          PhysicsEngine::ColliderRef colliderRefB) noexcept {
  if (colliderRefA.Index == ball_collider_refs_[0].Index) {
    const auto& col = world_.GetCollider(colliderRefA);
    auto& body = world_.GetBody(col.GetBodyRef());

    body.SetPosition(kCueBallStartPos);
    body.SetVelocity(Math::Vec2F::Zero());

    return;
  }

  if (colliderRefB.Index == ball_collider_refs_[0].Index) {
    const auto& col = world_.GetCollider(colliderRefB);
    auto& body = world_.GetBody(col.GetBodyRef());

    body.SetPosition(kCueBallStartPos);
    body.SetVelocity(Math::Vec2F::Zero());

    return;
  }

  const auto it_a = std::find(ball_collider_refs_.begin(),
                              ball_collider_refs_.end(), colliderRefA);

  if (it_a != ball_collider_refs_.end()) {
    auto& col = world_.GetCollider(*it_a);
    col.SetEnabled(false);
  }

  const auto it_b = std::find(ball_collider_refs_.begin(),
                              ball_collider_refs_.end(), colliderRefB);

  if (it_b != ball_collider_refs_.end()) {
    auto& col = world_.GetCollider(*it_b);
    col.SetEnabled(false);
  }
}
