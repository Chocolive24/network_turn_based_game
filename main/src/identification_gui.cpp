#include "identification_gui.h"

#include "client_application.h"

IdentificationGui::IdentificationGui() noexcept {
  font_.loadFromFile("data/Payback.otf");

  enter_username_txt_ = sf::Text("Enter your username: \n If you don't have one it will create it.", font_);
  enter_username_txt_.setCharacterSize(30);
  enter_username_txt_.setOrigin(enter_username_txt_.getGlobalBounds().width * 0.5f,
                         enter_username_txt_.getGlobalBounds().height * 0.5f);
  const auto pos = sf::Vector2f(ClientApplication::kWindowWidth_ * 0.5f,
                                ClientApplication::kWindowHeight_ * 0.33f);
  enter_username_txt_.setPosition(pos);
}

void IdentificationGui::Draw(sf::RenderTarget* render_target) noexcept {
  render_target->draw(enter_username_txt_);
}
