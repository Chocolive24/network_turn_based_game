#pragma once

#include "gui.h"

class LobbyGui final : public Gui {
public:
  explicit LobbyGui() noexcept;
  ~LobbyGui() noexcept override = default;
  void OnEvent(const sf::Event& event) noexcept override{}
  void Draw(sf::RenderTarget* render_target) noexcept override;

protected:
  void OnUpdate() noexcept override{}

private:
  sf::Font font_;
  sf::Text waiting_txt_{};
};