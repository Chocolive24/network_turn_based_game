#pragma once

#include "client.h"
#include "World.h"
#include "Timer.h"
#include "Metrics.h"
#include "constants.h"

#include <SFML/Network.hpp>
#include "SFML/Graphics.hpp"

#include <iostream>

class Game final : public PhysicsEngine::ContactListener{
public:
  Game() noexcept = default;
  ~Game() override = default;

  void Run() noexcept;
  void CreateWalls();

  void OnTriggerEnter(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnTriggerStay(PhysicsEngine::ColliderRef colliderRefA,
                     PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnTriggerExit(PhysicsEngine::ColliderRef colliderRefA,
                     PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnCollisionEnter(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override {
  }
  void OnCollisionExit(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override{}

 private:
  // Game attributes.
  // ----------------
  static constexpr std::int16_t kBallCount_ = 16;
  static constexpr float kPixelRadius_ = 17.5f;
  static constexpr float kMeterRadius_ = 0.175f;

  std::array<Math::Vec2F, kBallCount_> start_ball_pos_{};

  sf::Color wall_color_ = sf::Color::Blue;

  Math::Vec2F force_applied_to_ball_;

  bool has_game_started = false;
  bool has_played_ = false;
  bool is_player_turn_ = false;

  bool is_mouse_pressed_ = false;
  bool was_mouse_pressed_ = false;
  bool is_mouse_released_ = false;

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
  std::array<PhysicsEngine::BodyRef, kBallCount_> ball_body_refs_{};
  std::array<PhysicsEngine::ColliderRef, kBallCount_> ball_collider_refs_{};
  std::array<PhysicsEngine::BodyRef, 4> wall_body_refs_{};
  std::array<PhysicsEngine::ColliderRef, 4> wall_col_refs_{};

  // Graphics attributes.
  // --------------------
  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1000;

  static constexpr int kTriangleHeight_ = 300;

  sf::RenderWindow window_{};

  // Methods.
  // --------
  [[nodiscrad]] ReturnStatus Init() noexcept;
  void HandleWindowEvents();
  void Update() noexcept;
  void Draw() noexcept;
  void Deinit() noexcept;

  void CalculateStartBallPositions() noexcept;

  void CheckForReceivedPackets() noexcept;
};