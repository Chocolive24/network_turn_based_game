#include "gui.h"

void Gui::Update(sf::Vector2f mouse_pos) noexcept {
  mouse_pos_ = mouse_pos;

  OnUpdate();
}