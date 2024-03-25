#include "button.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Mouse.hpp>

Button::Button(sf::Vector2f pos, sf::Vector2f size, ButtonColor color, 
               const sf::Text& txt) noexcept {
  rect_ = sf::RectangleShape(size);
  rect_.setOrigin(size * 0.5f);
  rect_.setPosition(pos);
  rect_.setFillColor(color.base);

  color_ = color;
  text_ = txt;
}

bool Button::Draw(sf::Vector2f mouse_pos, sf::RenderTarget* target,
                  const sf::RenderStates& states) noexcept {
  const bool is_hovered = rect_.getGlobalBounds().contains(mouse_pos);

  const auto color = is_hovered ? color_.hovered : color_.base;
  rect_.setFillColor(color);

  target->draw(rect_, states);
  target->draw(text_, states);

  return is_hovered && sf::Mouse::isButtonPressed(sf::Mouse::Left);
}
