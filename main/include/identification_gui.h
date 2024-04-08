#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

#include "gui.h"

class IdentificationGui final : public Gui {
public:
  explicit IdentificationGui() noexcept;
 ~IdentificationGui() noexcept override = default;
  void OnEvent(const sf::Event& event) noexcept override{}
  void Draw(sf::RenderTarget* render_target) noexcept override;

protected:
  void OnUpdate() noexcept override{}

private:
  sf::Font font_;
  sf::Text enter_username_txt_{};
};