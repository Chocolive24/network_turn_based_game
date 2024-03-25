#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

struct ButtonColor {
  sf::Color base{};
  sf::Color hovered{};
};

/**
 * \brief Button is a class which represent a UI button and is drawn in immediate
 * mode. It means that the draw method return whether or not if the button
 * was clicked on.
 */
class Button {
public:
  explicit Button() noexcept = default;
  Button(sf::Vector2f pos, sf::Vector2f size, ButtonColor color,
         const sf::Text& txt) noexcept;

  /**
   * \brief Draw is a method which draws the button on the given render target and
   * checks if the button is clicked on or not.
   * This design pattern is called "immediate mode".
   * \param mouse_pos The position of the mouse relative to the window.
   * \param target The render target on which the button will be drawn.
   * \param states The render states of the button. By default it's Default state,
   * \return Whether or not the button is clicked on.
   */
  [[nodiscard]] bool Draw(sf::Vector2f mouse_pos, sf::RenderTarget* target,
    const sf::RenderStates& states = sf::RenderStates::Default) noexcept;

private:
  sf::RectangleShape rect_{};
  ButtonColor color_{};
  sf::Text text_{};
};
