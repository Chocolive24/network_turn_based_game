#pragma once

#include "graphics_manager.h"
#include "client.h"
#include "game.h"

#include <SFML/Graphics.hpp>

#include <iostream>

class ClientApplication {
public:
  ClientApplication(NetworkInterface* client) noexcept;

  [[nodiscard]] ReturnStatus Run() noexcept;

 private:
  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1000;
  static constexpr std::uint8_t kFrameRateLimit = 144;
  sf::RenderWindow window_{};

  NetworkInterface* client_ = nullptr;

  Game game_{};

  // Methods.
  // --------
  void Init() noexcept;
  void CheckForReceivedPackets() noexcept;
  void HandleWindowEvents();
  void LaunchLoop() noexcept;
  void Deinit() noexcept;
};