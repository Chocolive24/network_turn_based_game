#pragma once

#include "client_network_manager.h"
#include "game.h"
#include "gui.h"

#include <SFML/Graphics.hpp>

#include "client_identifier.h"

enum class ClientAppState : std::int8_t {
  kUserIdentification = 0,
  kInMainMenu,
  kInLobby,
  kInGame,
};

class ClientApplication {
public:
  ClientApplication(ClientNetworkInterface* client_net_interface) noexcept;

  [[nodiscard]] ReturnStatus Run() noexcept;

  void SendPacket(sf::Packet* packet) const noexcept;

  void SetState(ClientAppState state) noexcept { state_ = state; }

  [[nodiscard]] const PlayerData& player_data() const noexcept {
    return player_data_;
  }

  static constexpr int kWindowWidth_ = 750;
  static constexpr int kWindowHeight_ = 1000;

 private:
  void Init() noexcept;
  void PollWindowEvents() noexcept;
  void PollNetworkEvents() noexcept;
  void LaunchLoop() noexcept;
  void OnClientIdentified(std::string_view username) const noexcept;
  void Deinit() noexcept;

  static constexpr std::uint8_t kFrameRateLimit_ = 144;
  sf::RenderWindow window_{};

  ClientNetworkInterface* client_network_interface_ = nullptr;
  std::unique_ptr<Gui> current_gui_ = nullptr;

  ClientIdentifier client_identifier_{};
  PlayerData player_data_{};
  //std::string username_{};
  //int elo_ = 0;

  Game game_{};

  ClientAppState state_ = ClientAppState::kUserIdentification;
  
  
  sf::Font font_{};
};