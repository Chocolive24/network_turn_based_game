#pragma once

#include "game.h"
#include "graphics_interface.h"

#include "SFML/Graphics.hpp"

/**
 * \headerfile graphics_manager.h
 * This header defines the GameGraphicsManager is a class that draws the game,
 * inheriting the GraphicsInterface and the Game.
 * \author Olivier Pachoud
 */



/**
 * \brief GameGraphicsManager is a class that draws the game,
 * inheriting the DrawInterface and the GameManager.
 */
class GameGraphicsManager final : public GraphicsInterface {
public:
  GameGraphicsManager() noexcept = default;
  GameGraphicsManager(GameGraphicsManager&& other) noexcept = default;
  GameGraphicsManager& operator=(GameGraphicsManager&& other) noexcept =
      default;
  GameGraphicsManager(const GameGraphicsManager& other) noexcept = default;
  GameGraphicsManager& operator=(const GameGraphicsManager& other) noexcept =
      default;
  ~GameGraphicsManager() noexcept override = default;

  void Init(sf::RenderTarget* render_target) noexcept override;
  void Draw() noexcept override;

private:
  void DrawChargingRect() const noexcept;
  void DrawTable() const noexcept;
  void DrawBalls() noexcept;
  void DrawWalls() noexcept;
  void DrawHoles() noexcept;

  void DrawUi() const noexcept;

private:
  sf::RenderTarget* render_target_ = nullptr;

  sf::Color wall_color_ = sf::Color::Blue;
  sf::Font font_{};
};