#pragma once

#include "button.h"
#include "gui.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Event.hpp>

class ClientApplication;

class MainMenuGui final : public Gui {
 public:
  explicit MainMenuGui(ClientApplication* client_app) noexcept;

  void OnEvent(const sf::Event& event) noexcept override;
  void Draw(sf::RenderTarget* render_target) noexcept override;

 private:
  void OnUpdate() noexcept override;

  ClientApplication* client_app_ = nullptr;
  Button play_button_{};
  sf::Font font_{};
};