#include "lobby_gui.h"

#include "client_application.h"

LobbyGui::LobbyGui() noexcept {
  font_.loadFromFile("data/Payback.otf");

  waiting_txt_ = sf::Text("Waiting for an other player...", font_);
  waiting_txt_.setCharacterSize(30);
  waiting_txt_.setOrigin(waiting_txt_.getGlobalBounds().width * 0.5f,
                         waiting_txt_.getGlobalBounds().height * 0.5f);
  const auto pos = sf::Vector2f(ClientApplication::kWindowWidth_ * 0.5f,
                                ClientApplication::kWindowHeight_ * 0.33f);
  waiting_txt_.setPosition(pos);
}

void LobbyGui::Draw(sf::RenderTarget* render_target) noexcept {
  render_target->draw(waiting_txt_);
}
