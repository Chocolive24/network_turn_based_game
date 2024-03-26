#include "gui.h"

void Gui::Update(const sf::Vector2f mouse_pos) noexcept {
  mouse_pos_ = mouse_pos;

  OnUpdate();
}