#include "game_graphics_manager.h"

void GameGraphicsManager::Init() noexcept {
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(kWindowWidth_, kWindowHeight_), kWindowTitle_.data(),
                 sf::Style::Close, window_settings);
  window_.setVerticalSyncEnabled(true);

  font_.loadFromFile("data/Payback.otf");
}

void GameGraphicsManager::Draw() noexcept {}
