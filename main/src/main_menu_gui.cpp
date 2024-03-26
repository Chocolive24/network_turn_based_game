#include "main_menu_gui.h"
#include "packet.h"
#include "client_application.h"

MainMenuGui::MainMenuGui(ClientApplication* client_app) noexcept {
  client_app_ = client_app;

  font_.loadFromFile("data/Payback.otf");

  const auto pos = sf::Vector2f(ClientApplication::kWindowWidth_ * 0.5f,
                                ClientApplication::kWindowHeight_ * 0.5f);
  const auto size = sf::Vector2f(0.5f * ClientApplication::kWindowWidth_,
                                 0.1f * ClientApplication::kWindowHeight_);
  const ButtonColor color{sf::Color::Blue, sf::Color::Green};
  sf::Text txt("Play", font_);
  txt.setCharacterSize(30);
  txt.setOrigin(txt.getGlobalBounds().width * 0.5f,
                txt.getGlobalBounds().height * 0.5f);
  txt.setPosition(pos);

  play_button_ = Button(pos, size, color, txt);

  const auto& player_data = client_app_->player_data();
  const std::string data_str = player_data.username + " elo: " + 
    std::to_string(player_data.elo);

  player_data_txt_.setString(data_str);
  player_data_txt_.setFont(font_);
  player_data_txt_.setCharacterSize(30);
  player_data_txt_.setOrigin(player_data_txt_.getGlobalBounds().width * 0.5f,
                             player_data_txt_.getGlobalBounds().height * 0.5f);
  player_data_txt_.setPosition(ClientApplication::kWindowWidth_ * 0.5f,
                  ClientApplication::kWindowHeight_ * 0.33f);
}                 

void MainMenuGui::OnEvent(const sf::Event& event) noexcept {}

void MainMenuGui::OnUpdate() noexcept {}

void MainMenuGui::Draw(sf::RenderTarget* render_target) noexcept {
  if (play_button_.Draw(mouse_pos_, render_target)) {
    sf::Packet join_lobby_packet;
    join_lobby_packet << PacketType::kJoinLobby << client_app_->player_data().username;
    client_app_->SendPacket(&join_lobby_packet);
  }

  const auto& player_data = client_app_->player_data();
  const std::string data_str =
      player_data.username + "\n elo: " + std::to_string(player_data.elo);

  player_data_txt_.setOrigin(player_data_txt_.getGlobalBounds().width * 0.5f,
                             player_data_txt_.getGlobalBounds().height * 0.5f);
  player_data_txt_.setString(data_str);

  render_target->draw(player_data_txt_);
}