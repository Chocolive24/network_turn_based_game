#pragma once

#include <SFML/Graphics/Drawable.hpp>

class Button final : public sf::Drawable {
public:
  ~Button() override;

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};