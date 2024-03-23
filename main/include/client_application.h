#pragma once

#include "client_network_manager.h"
#include "game.h"

#include <SFML/Graphics.hpp>

enum class ClientAppState : std::int8_t {
  KEntryPoint,
  kInLobby,
  kInGame,
};

class ClientApplication {
public:
  ClientApplication(ClientNetworkInterface* client_net_interface) noexcept;

  [[nodiscard]] ReturnStatus Run() noexcept;

 private:
  void Init() noexcept;
  void CheckForReceivedPackets() noexcept;
  void PollWindowEvents();
  void LaunchLoop() noexcept;
  void Deinit() noexcept;

  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1000;
  static constexpr std::uint8_t kFrameRateLimit = 144;
  sf::RenderWindow window_{};

  ClientNetworkInterface* client_network_interface_ = nullptr;

  Game game_{};

  ClientAppState state_ = ClientAppState::KEntryPoint;
};