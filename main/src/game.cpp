#include "game.h"

void Game::Init() noexcept {
  timer_.Init();
  world_.Init(Math::Vec2F::Zero(), 15);

  // Create window.
  // --------------
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  window_.create(sf::VideoMode(1280, 720), "Pool",
                sf::Style::Default, window_settings);
  window_.setVerticalSyncEnabled(true);
}

void Game::Run() noexcept {
  Init();


}
