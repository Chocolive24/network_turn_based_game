#include "game.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <iomanip>

void Game::InitGame(NetworkInterface* client,
                    sf::RenderTarget* render_target,
                    Math::Vec2F window_size) noexcept {
  client_ = client;
  render_target_ = render_target;
  window_size_ = window_size;

  timer_.Init();
  world_.Init(Math::Vec2F::Zero(), kBallCount_);
  world_.SetContactListener(this);

  font_.loadFromFile("data/Payback.otf");

  CreateBalls();
  CreateWalls();
  CreateHoles();
}

void Game::OnPacketReceived(sf::Packet* packet, PacketType packet_type) noexcept {
  switch (packet_type) {
    case PacketType::KCueBallVelocity: {
      *packet >> force_applied_to_ball_.X >>
          force_applied_to_ball_.Y;
      std::cout << std::setprecision(200) << "Received "
                << force_applied_to_ball_.X << " "
                << force_applied_to_ball_.Y << '\n';
      const auto cue_ball_b_ref =
          world_.GetCollider(ball_collider_refs_[0])
              .GetBodyRef();
      auto& cue_ball_body = world_.GetBody(cue_ball_b_ref);
      cue_ball_body.SetVelocity(force_applied_to_ball_);
      break;
    }
    case PacketType::KStartGame: {
      *packet >> has_game_started >> player_index_;
      std::cout << "GAME STARTED\n";
      break;
    }
    case PacketType::kNewTurn: {
      std::cout << "NEW TURN\n";
      *packet >> is_player_turn_;
      const auto cue_ball_b_ref =
          world_.GetCollider(ball_collider_refs_[0])
              .GetBodyRef();
      auto& cue_ball_body = world_.GetBody(cue_ball_b_ref);
      cue_ball_body.SetVelocity(Math::Vec2F::Zero());
      break;
    }
    case PacketType::kBallStateCorrections: {
      for (const auto& col_ref : ball_collider_refs_) {
        const auto& body_ref =
            world_.GetCollider(col_ref).GetBodyRef();
        auto& body = world_.GetBody(body_ref);
        Math::Vec2F ball_pos = Math::Vec2F::Zero();
        bool enabled = false;
        *packet >> ball_pos.X >> ball_pos.Y >> enabled;
        body.SetPosition(ball_pos);
        world_.GetCollider(col_ref).SetEnabled(enabled);
      }
      break;
    }
    default:
      break;
  }
}

void Game::Update(Math::Vec2F mouse_pos) noexcept {
  //OnPacketReceived(TODO);

  mouse_pos_ = mouse_pos;

  timer_.Tick();

  is_mouse_just_pressed_ =
      was_mouse_pressed_ == false && is_mouse_pressed_ == true;

  if (!is_game_finished_) {
    HandlePlayerTurn();

    world_.Update(kFixedTimeStep);

    CheckEndTurnCondition();

    UpdateScores();
  }

  was_mouse_pressed_ = is_mouse_pressed_;
}

void Game::OnEvent(const sf::Event& event) noexcept {
  switch (event.type) {
    case sf::Event::MouseButtonPressed:
      is_mouse_pressed_ = true;
      is_mouse_released_ = false;
      break;
    case sf::Event::MouseButtonReleased:
      is_mouse_released_ = true;
      is_mouse_pressed_ = false;
      break;
    case sf::Event::KeyPressed:
      switch (event.key.code) {
        case sf::Keyboard::W: {
          static int idx = 2 + player_index_;
          if (idx > 15) idx = 1;
          const auto& b_ref =
              world_.GetCollider(ball_collider_refs_[idx]).GetBodyRef();
          auto& b = world_.GetBody(b_ref);
          b.SetPosition(world_.GetBody(hole_body_refs_[0]).Position());
          idx += 2;
          break;
        }
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void Game::Deinit() noexcept { world_.Deinit(); }

void Game::Draw() noexcept {
  const sf::Color clear_color =
      player_index_ == 0 ? sf::Color::Blue : sf::Color::Red;
  render_target_->clear(clear_color);

  // Draw gameplay objects.
  // ----------------------
  DrawTable();
  DrawWalls();
  DrawHoles();
  DrawChargingRect();
  DrawBalls();

  // Draw UI.
  // --------
  DrawUi();
}

void Game::CreateBalls() noexcept {
  kCueBallStartPos = Metrics::PixelsToMeters(
      Math::Vec2F(window_size_.X * 0.5f, window_size_.Y * 0.66f));
  ball_collider_refs_.reserve(kBallCount_);

  const Math::Vec2F window_size_meters = Metrics::PixelsToMeters(window_size_);

  start_ball_pos_[0] = kCueBallStartPos;

  constexpr int maxHeight = 5;
  float height = 0;
  const Math::Vec2F startPos =
      Math::Vec2F(window_size_meters.X * 0.5f, window_size_meters.Y * 0.4f);

  int index = 1;

  static constexpr float space = kMeterRadius_ * 2.f;

  for (int i = 1; i <= maxHeight; i++) {
    auto startPosition =
        startPos + Math::Vec2F(-((i - 1) * space) / 2.f, height);

    for (int nbBall = 0; nbBall < i; nbBall++) {
      start_ball_pos_[index] = (startPosition + Math::Vec2F(nbBall * space, 0));
      index++;
    }

    height += space * std::sqrt(3.0f) / 2.0f;
  }

  for (int i = 0; i < kBallCount_; i++) {
    const auto body_ref = world_.CreateBody();
    auto& body = world_.GetBody(body_ref);
    body.SetBodyType(PhysicsEngine::BodyType::Dynamic);
    const float f_i = static_cast<float>(i) * 0.5f;
    body.SetPosition(start_ball_pos_[i]);
    body.SetDamping(0.5f);

    ball_collider_refs_.push_back(world_.CreateCollider(body_ref));
    auto& collider = world_.GetCollider(ball_collider_refs_[i]);
    collider.SetShape(Math::CircleF(Math::Vec2F::Zero(), kMeterRadius_));
    collider.SetRestitution(0.25f);
  }
}

void Game::CreateWalls() noexcept {
  auto win_size_meter = Metrics::PixelsToMeters(window_size_);

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
  col_0.SetRestitution(0.25f);

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
  auto win_size_meter = Metrics::PixelsToMeters(window_size_);

  float x_pos = win_size_meter.X - 0.725f;
  constexpr float start_y_pos = -0.875f;
  float y_pos = start_y_pos;

  for (int i = 0; i < 6; i++) {
    hole_body_refs_[i] = world_.CreateBody();
    auto& body_0 = world_.GetBody(hole_body_refs_[i]);
    body_0.SetBodyType(PhysicsEngine::BodyType::Kinematic);
    body_0.SetPosition(Math::Vec2F(x_pos, y_pos));

    hole_col_refs_[i] = world_.CreateCollider(hole_body_refs_[i]);
    auto& col_0 = world_.GetCollider(hole_col_refs_[i]);
    col_0.SetShape(Math::CircleF(Math::Vec2F::Zero(), 0.15f));
    col_0.SetIsTrigger(true);

    y_pos += win_size_meter.Y * 0.5f - start_y_pos;

    if (i == 2) {
      x_pos = 0.725f;
      y_pos = start_y_pos;
    }
  }
}

void Game::HandlePlayerTurn() noexcept {
  const auto cue_ball_body_ref =
      world_.GetCollider(ball_collider_refs_[0]).GetBodyRef();
  auto& cue_ball_body = world_.GetBody(cue_ball_body_ref);

  if (is_player_turn_ && !has_played_) {
    /*const auto mouse_pos = Math::Vec2F(sf::Mouse::getPosition(window_).x,
                                       sf::Mouse::getPosition(window_).y);*/
    const auto ball_pos_in_pix = Metrics::MetersToPixels(
        Math::Vec2F(cue_ball_body.Position().X, cue_ball_body.Position().Y));
    const auto ball_to_mouse_vec = mouse_pos_ - ball_pos_in_pix;

    if (ball_to_mouse_vec.Length<float>() <= kPixelRadius_ &&
        is_mouse_just_pressed_) {
      is_charging = true;
    }

    if (is_charging) {
      const auto distance = ball_to_mouse_vec.Length() > max_aim_dist
                                ? max_aim_dist
                                : ball_to_mouse_vec.Length();

      force_percentage_ = distance / max_aim_dist;

      const auto aim_direction = -ball_to_mouse_vec.Normalized();

      // Set up rectangle properties
      charging_rect_ = sf::RectangleShape(
          sf::Vector2f(distance * 0.5f + kPixelRadius_, 10.f));
      charging_rect_.setOrigin(0.f, charging_rect_.getSize().y * 0.5f);
      charging_rect_.setPosition(
          sf::Vector2f(ball_pos_in_pix.X, ball_pos_in_pix.Y));
      const float angle =
          std::atan2(aim_direction.Y, aim_direction.X) * 180.f / 3.14159265f;
      charging_rect_.setRotation(angle);

      if (is_mouse_released_) {
        is_charging = false;

        force_applied_to_ball_ = Metrics::PixelsToMeters(
            force_percentage_ * max_amplitude_ * aim_direction);

        cue_ball_body.SetVelocity(Math::Vec2F(force_applied_to_ball_));
        sf::Packet force_applied_packet;
        force_applied_packet << PacketType::KCueBallVelocity
                             << force_applied_to_ball_.X
                             << force_applied_to_ball_.Y;
        client_->SendPacket(force_applied_packet);

        std::cout << std::setprecision(200)
                  << "Sent: " << force_applied_to_ball_.X << " "
                  << force_applied_to_ball_.Y << '\n';

        force_percentage_ = 0.f;

        has_played_ = true;
      }
    }
  }
}

void Game::CheckEndTurnCondition() noexcept {
  Math::Vec2F global_ball_velocities = Math::Vec2F::Zero();

  for (const auto& col_ref : ball_collider_refs_) {
    const auto& col = world_.GetCollider(col_ref);
    if (!col.Enabled()) continue;

    const auto body_ref = col.GetBodyRef();
    auto& body = world_.GetBody(body_ref);
    global_ball_velocities += body.Velocity();
  }

  if (has_played_ && global_ball_velocities.Length<float>() <= Math::Epsilon) {
    sf::Packet ball_pos_packet;
    ball_pos_packet << PacketType::kBallStateCorrections;
    for (const auto& col_ref : ball_collider_refs_) {
      const auto& body_ref = world_.GetCollider(col_ref).GetBodyRef();
      auto& body = world_.GetBody(body_ref);

      ball_pos_packet << body.Position().X << body.Position().Y
                      << world_.GetCollider(col_ref).Enabled();
    }
    client_->SendPacket(ball_pos_packet);

    sf::Packet new_turn_packet;
    new_turn_packet << PacketType::kNewTurn << true;
    client_->SendPacket(new_turn_packet);
    is_player_turn_ = false;
    has_played_ = false;
  }
}

void Game::UpdateScores() noexcept {
  if (is_game_finished_) return;

  std::int16_t ball_scored_count = 0;
  std::int16_t opponent_ball_scored_count = 0;

  for (int i = 2; i < kBallCount_; i++) {
    if (!world_.GetCollider(ball_collider_refs_[i]).Enabled()) {
      if (i % 2 == player_index_) {
        ball_scored_count++;
      } else {
        opponent_ball_scored_count++;
      }
    }
  }

  score_ = ball_scored_count;
  opponent_score_ = opponent_ball_scored_count;
}

void Game::OnTriggerEnter(
    PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept {
  if (colliderRefA.Index == ball_collider_refs_[0].Index ||
      colliderRefB.Index == ball_collider_refs_[0].Index) {
    const auto& col = world_.GetCollider(
        colliderRefA.Index == ball_collider_refs_[0].Index ? colliderRefA
                                                           : colliderRefB);
    auto& body = world_.GetBody(col.GetBodyRef());
    body.SetPosition(kCueBallStartPos);
    body.SetVelocity(Math::Vec2F::Zero());
    force_applied_to_ball_ = Math::Vec2F::Zero();
    return;
  }

  if (colliderRefA.Index == 1 || colliderRefB.Index == 1) {
    auto& col = world_.GetCollider(
        colliderRefA.Index == ball_collider_refs_[1].Index ? colliderRefA
                                                           : colliderRefB);
    col.SetEnabled(false);
    is_game_finished_ = true;

    if (is_player_turn_ && score_ == 7) {
      score_++;
      has_win_ = true;

      return;
    }

    if (!is_player_turn_ && opponent_score_ != 7) {
      has_win_ = true;
    }

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

void Game::DrawChargingRect() const noexcept {
  if (is_charging) {
    render_target_->draw(charging_rect_);
  }
}

void Game::DrawTable() const noexcept {
  const sf::Vector2f table_size(window_size_.X - 50.f, window_size_.Y - 50.f);
  sf::RectangleShape table_rect(table_size);
  table_rect.setPosition(window_size_.X * 0.5f, window_size_.Y * 0.5f);
  table_rect.setOrigin(table_size.x * 0.5f, table_size.y * 0.5f);
  table_rect.setFillColor(sf::Color(76, 153, 0));
  render_target_->draw(table_rect);
}

void Game::DrawBalls() noexcept {
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
      const auto inv_f_percentage = std::abs(force_percentage_ - 1.f);
      color = sf::Color(255, 255 * inv_f_percentage, 255 * inv_f_percentage);
    } else if (i == 1) {
      color = sf::Color::Yellow;
    } else {
      color = i % 2 == 0 ? sf::Color::Blue : sf::Color::Red;
    }

    circle.setFillColor(color);
    render_target_->draw(circle);
    i++;
  }
}

void Game::DrawWalls() noexcept {
  for (std::int16_t i = 0; i < 4; i++) {
    const auto& col = world_.GetCollider(wall_col_refs_[i]);
    const auto rect_col =
        Metrics::MetersToPixels(std::get<Math::RectangleF>(col.Shape()).Size());
    sf::RectangleShape rect(sf::Vector2f(rect_col.X, rect_col.Y));

    rect.setOrigin(rect_col.X * 0.5f, rect_col.Y * 0.5f);

    auto& body = world_.GetBody(wall_body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
    rect.setPosition(pixel_pos.X, pixel_pos.Y);

    wall_color_ = is_player_turn_ ? sf::Color::Green : sf::Color::Magenta;
    rect.setFillColor(sf::Color(102, 51, 0));

    render_target_->draw(rect);
  }
}

void Game::DrawHoles() noexcept {
  for (std::int16_t i = 0; i < 6; i++) {
    auto& col = world_.GetCollider(hole_col_refs_[i]);
    const auto radius = Metrics::MetersToPixels(
        std::get<Math::CircleF>(col.Shape()).Radius() * 2.f);

    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);

    auto& body = world_.GetBody(hole_body_refs_[i]);
    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
    circle.setPosition(pixel_pos.X, pixel_pos.Y);

    circle.setFillColor(sf::Color::Black);
    render_target_->draw(circle);
  }
}

void Game::DrawUi() const noexcept {
  sf::FloatRect text_bounds{};

  sf::Text score_txt(std::to_string(score_), font_);
  score_txt.setPosition(150, 50);
  text_bounds = score_txt.getLocalBounds();
  score_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
  render_target_->draw(score_txt);

  sf::Text opp_score_txt(std::to_string(opponent_score_), font_);
  opp_score_txt.setPosition(window_size_.X - 150, 50);
  text_bounds = opp_score_txt.getLocalBounds();
  opp_score_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
  render_target_->draw(opp_score_txt);

  const auto turn_string = is_player_turn_ ? "Your turn" : "Opponent's turn";
  sf::Text turn_txt(turn_string, font_);
  turn_txt.setPosition(window_size_.X * 0.5f, 50.f);
  text_bounds = turn_txt.getLocalBounds();
  turn_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
  render_target_->draw(turn_txt);

  if (is_game_finished_) {
    const std::string end_game_str = has_win_ ? "Victory !" : "Defeat !";
    sf::Text end_game_txt(end_game_str, font_);
    end_game_txt.setPosition(window_size_.X * 0.5f, window_size_.Y * 0.5f);
    text_bounds = end_game_txt.getLocalBounds();
    end_game_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
    render_target_->draw(end_game_txt);
  }
}
