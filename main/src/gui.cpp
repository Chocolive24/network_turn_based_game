#include "gui.h"

void Gui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  OnDraw(target, states);
}
