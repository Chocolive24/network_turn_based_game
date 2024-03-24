#include "gui.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Mouse.hpp>

#include "client_application.h"

#include "packet.h"

void Gui::Update(sf::Vector2f mouse_pos) noexcept {
  mouse_pos_ = mouse_pos;

  OnUpdate();
}

void MainMenuGui::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  sf::RectangleShape play_button(
      sf::Vector2f(0.5f * ClientApplication::kWindowWidth_, 0.1f * ClientApplication::kWindowHeight_));
  play_button.setOrigin(play_button.getSize() * 0.5f);
  play_button.setPosition(ClientApplication::kWindowWidth_ * 0.5f, ClientApplication::kWindowHeight_ * 0.33f);

  const bool is_hover = play_button.getGlobalBounds().contains(mouse_pos_);

  const auto color = is_hover ? sf::Color::Green : sf::Color::Blue;
  play_button.setFillColor(color);

  if (is_hover && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
    sf::Packet join_lobby_packet;
    join_lobby_packet << PacketType::kJoinLobby;
    client_app_->SendPacket(&join_lobby_packet);
  }

  target.draw(play_button);
}

MainMenuGui::MainMenuGui(ClientApplication* client_app) noexcept {
  client_app_ = client_app;
}

void MainMenuGui::OnEvent(const sf::Event& event) noexcept {
  //switch (event.type)
  //{
  //      
  //}
}

void MainMenuGui::OnUpdate() noexcept {
  
}
