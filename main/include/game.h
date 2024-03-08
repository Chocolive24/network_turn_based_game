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

  void OnTriggerEnter(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnTriggerStay(PhysicsEngine::ColliderRef colliderRefA,
                     PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnTriggerExit(PhysicsEngine::ColliderRef colliderRefA,
                     PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnCollisionEnter(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override{}
  void OnCollisionExit(
      PhysicsEngine::ColliderRef colliderRefA,
      PhysicsEngine::ColliderRef colliderRefB) noexcept override{}

 private:
  // Game attributes.
  // ----------------
  static constexpr std::int16_t kBallCount_ = 16;
  static constexpr float kPixelRadius_ = 15.f;
  static constexpr float kMeterRadius_ = 0.15f;

  std::array<Math::Vec2F, kBallCount_> start_ball_pos_{};

  sf::Vector2i other_circle_pos_;

  bool is_player_turn_ = false;

  bool is_mouse_pressed_ = false;
  bool was_mouse_pressed_ = false;
  bool is_mouse_released_ = false;

  bool is_charging = false;

  // Common attributes.
  // ------------------
  Timer timer_{};

  // Network attributes.
  // -------------------
  Client client_;

  // Physics attributes.
  // -------------------
  PhysicsEngine::World world_{};
  std::array<PhysicsEngine::BodyRef, kBallCount_> body_refs_{};
  std::array<PhysicsEngine::ColliderRef, kBallCount_> collider_refs_{};

  // Graphics attributes.
  // --------------------
  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1080;

  static constexpr int kTriangleHeight_ = 300;

  sf::RenderWindow window_{};

  // Methods.
  // --------
  [[nodiscrad]] ReturnStatus Init() noexcept;
  void Update() noexcept;
  void Draw() noexcept;
  void Deinit() noexcept;

  void CalculateStartBallPositions() noexcept;

  void CheckForReceivedPackets() noexcept;
};