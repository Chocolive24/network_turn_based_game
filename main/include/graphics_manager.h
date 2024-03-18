/**
 * \headerfile graphics_manager.h
 * This header defines the GraphicsManager class which draws the
 * data given by the game using the DrawInterface methods and SFML.
 * \author Olivier Pachoud
 */

#pragma once

#include "graphics_interface.h"

#include "SFML/Graphics.hpp"

/**
 * \brief GraphicsManager is a class which draws the
 * data given by the game using the GraphicsInterface methods and SFML
 */
class GraphicsManager : DrawInterface {
public:
  ~GraphicsManager() noexcept override = default;
  void Draw() noexcept override;

private:
  static constexpr std::string_view kWindowTitle_ = "8-Ball Pool";
  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1000;

  sf::RenderWindow window_;
  sf::Font font_;
};