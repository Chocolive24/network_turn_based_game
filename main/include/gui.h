#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Event.hpp>

class Gui {
public:
  virtual ~Gui() noexcept = default;

  void Update(sf::Vector2f mouse_pos) noexcept;

  virtual void OnEvent(const sf::Event& event) noexcept = 0;
  virtual void Draw(sf::RenderTarget* render_target) noexcept = 0;

protected:
  virtual void OnUpdate() noexcept = 0;

  sf::Vector2f mouse_pos_{};
};