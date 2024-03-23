#include "gui.h"

void Gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  OnDraw(target, states);
}

void EntryPointGui::OnDraw(sf::RenderTarget& target, sf::RenderStates states) const noexcept {
  
}
