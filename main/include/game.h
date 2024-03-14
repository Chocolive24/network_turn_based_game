#pragma once

#include "client.h"
#include "World.h"
#include "Timer.h"
#include "Metrics.h"
#include "constants.h"

#include <SFML/Network.hpp>
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include <iostream>

class Game final : public PhysicsEngine::ContactListener{
public:
  Game() noexcept = default;
  ~Game() override = default;

  [[nodiscard]] ReturnStatus Run() noexcept;

 private:
  // Game attributes.
  // ----------------
  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1000;
  static constexpr int kTriangleHeight_ = 300;

  static constexpr Math::Vec2F kWindowSizeInMeters =
      Metrics::PixelsToMeters(Math::Vec2F(kWindowWidth_, kWindowHeight_));
  static constexpr std::int16_t kBallCount_ = 16;
  static constexpr float kPixelRadius_ = 17.5f;
  static constexpr float kMeterRadius_ = 0.175f;
  static constexpr Math::Vec2F kCueBallStartPos =
      Math::Vec2F(kWindowSizeInMeters.X * 0.5f, kWindowSizeInMeters.Y * 0.66f);

  static constexpr float max_amplitude_ = 1000.f;
  static constexpr float max_aim_dist = 200.f;

  float force_percentage_ = 0.f;
  sf::RectangleShape charging_rect_{};

  int player_index_ = -1;
  std::int16_t score_ = 0;
  std::int16_t opponent_score_ = 0;
  bool has_win_ = false;

  std::array<Math::Vec2F, kBallCount_> start_ball_pos_{};

  sf::Color wall_color_ = sf::Color::Blue;

  Math::Vec2F force_applied_to_ball_;

  bool has_game_started = false;
  bool is_game_finished_ = false;
  bool has_played_ = false;
  bool is_player_turn_ = false;
  bool can_replay_ = false;

  bool is_mouse_pressed_ = false;
  bool was_mouse_pressed_ = false;
  bool is_mouse_released_ = false;
  bool is_mouse_just_pressed_ = false;

  bool is_charging = false;

  // Common attributes.
  // ------------------
  static constexpr float kFixedTimeStep = 1.f / 60.f;
  Timer timer_{};

  // Network attributes.
  // -------------------
  Client client_;

  // Physics attributes.
  // -------------------
  PhysicsEngine::World world_{};
  //std::vector<PhysicsEngine::BodyRef> ball_body_refs_{};
  std::vector<PhysicsEngine::ColliderRef> ball_collider_refs_{};
  std::array<PhysicsEngine::BodyRef, 4> wall_body_refs_{};
  std::array<PhysicsEngine::ColliderRef, 4> wall_col_refs_{};
  std::array<PhysicsEngine::BodyRef, 6> hole_body_refs_{};
  std::array<PhysicsEngine::ColliderRef, 6> hole_col_refs_{};

  // Graphics attributes.
  // --------------------
  sf::RenderWindow window_{};

  sf::Font font_{};

  // Methods.
  // --------
  [[nodiscrad]] ReturnStatus Init() noexcept;
  void CheckForReceivedPackets() noexcept;
  void HandlePlayerTurn();
  void CheckEndTurnCondition();
  void HandleWindowEvents();
  void Update() noexcept;
  void DrawTexts();
  void Draw() noexcept;
  void Deinit() noexcept;

  void CreateBalls() noexcept;
  void CreateWalls();
  void CreateHoles() noexcept;

  void UpdateScores() noexcept;

  void DrawBalls();
  void DrawWalls();
  void DrawHoles();

  void OnTriggerEnter(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override;
  void OnTriggerStay(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override {}
  void OnTriggerExit(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override {}
  void OnCollisionEnter(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override {}
  void OnCollisionExit(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override {}
};