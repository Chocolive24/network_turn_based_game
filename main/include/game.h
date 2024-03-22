#pragma once

#include "World.h"
#include "Timer.h"
#include "Metrics.h"
#include "client_network_interface.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

/**
 * \brief GameManager is a class that update the state of the game.
 */
class Game : public PhysicsEngine::ContactListener {
 public:
  void InitGame(ClientNetworkInterface* client, sf::RenderTarget* render_target,
                Math::Vec2F window_size) noexcept;
  void OnPacketReceived(sf::Packet* packet, PacketType packet_type) noexcept;
  void Update(Math::Vec2F mouse_pos) noexcept;
  void OnEvent(const sf::Event& event) noexcept;
  void Deinit() noexcept;

  void Draw() noexcept;

  static constexpr int kStartBallTriangleHeight_ = 300;
  static constexpr std::int16_t kBallCount_ = 16;
  static constexpr float kPixelRadius_ = 17.5f;
  static constexpr float kMeterRadius_ = 0.175f;
  static constexpr float max_amplitude_ = 1000.f;
  static constexpr float max_aim_dist = 200.f;

  sf::RenderTarget* render_target_ = nullptr;

  ClientNetworkInterface* client_ = nullptr;

  sf::Color wall_color_ = sf::Color::Blue;
  sf::Font font_{};

  Math::Vec2F window_size_ = Math::Vec2F::Zero();
  Math::Vec2F mouse_pos_ = Math::Vec2F::Zero();

  Math::Vec2F kCueBallStartPos = Math::Vec2F::Zero();

  float force_percentage_ = 0.f;
  sf::RectangleShape charging_rect_{};

  int player_index_ = -1;
  std::int16_t score_ = 0;
  std::int16_t opponent_score_ = 0;
  bool has_win_ = false;

  std::array<Math::Vec2F, kBallCount_> start_ball_pos_{};

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
  Timer timer_{};
  static constexpr float kFixedTimeStep = 1.f / 50.f;

  // Physics attributes.
  // -------------------
  PhysicsEngine::World world_{};
  std::vector<PhysicsEngine::ColliderRef> ball_collider_refs_{};
  std::array<PhysicsEngine::BodyRef, 4> wall_body_refs_{};
  std::array<PhysicsEngine::ColliderRef, 4> wall_col_refs_{};
  std::array<PhysicsEngine::BodyRef, 6> hole_body_refs_{};
  std::array<PhysicsEngine::ColliderRef, 6> hole_col_refs_{};

private:
  void CreateBalls() noexcept;
  void CreateWalls() noexcept;
  void CreateHoles() noexcept;

  void HandlePlayerTurn() noexcept;
  void CheckEndTurnCondition() noexcept;
  void UpdateScores() noexcept;

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

  void DrawChargingRect() const noexcept;
  void DrawTable() const noexcept;
  void DrawBalls() noexcept;
  void DrawWalls() noexcept;
  void DrawHoles() noexcept;

  void DrawUi() const noexcept;
};