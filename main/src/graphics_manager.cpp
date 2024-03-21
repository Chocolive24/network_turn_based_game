#include "graphics_manager.h"

void GameGraphicsManager::Init(sf::RenderTarget* render_target) noexcept {
  render_target_ = render_target;

  font_.loadFromFile("data/Payback.otf");
}

void GameGraphicsManager::Draw() noexcept {
  //const sf::Color clear_color = player_index_ == 0 ? sf::Color::Blue : sf::Color::Red;
  //render_target_->clear(clear_color);

  //// Draw gameplay objects.
  //// -----------------
  //DrawChargingRect();
  //DrawTable();
  //DrawBalls();
  //DrawWalls();
  //DrawHoles();

  //// Draw UI.
  //// --------
  //DrawUi();
}

//void GameGraphicsManager::DrawChargingRect() const noexcept {
//  if (is_charging) {
//    render_target_->draw(charging_rect_);
//  }
//}
//
//void GameGraphicsManager::DrawTable() const noexcept {
//  const sf::Vector2f table_size(window_size_.X - 50.f, window_size_.Y - 50.f);
//  sf::RectangleShape table_rect(table_size);
//  table_rect.setPosition(window_size_.X * 0.5f, window_size_.Y * 0.5f);
//  table_rect.setOrigin(table_size.x * 0.5f, table_size.y * 0.5f);
//  table_rect.setFillColor(sf::Color(76, 153, 0));
//  render_target_->draw(table_rect);
//}
//
//void GameGraphicsManager::DrawBalls() noexcept {
//  int i = 0;
//  for (const auto& col_ref : ball_collider_refs_) {
//    const auto& col = world_.GetCollider(col_ref);
//    if (!col.Enabled()) {
//      i++;
//      continue;
//    }
//
//    sf::CircleShape circle(kPixelRadius_);
//    circle.setOrigin(kPixelRadius_, kPixelRadius_);
//
//    auto& ball_body = world_.GetBody(col.GetBodyRef());
//    const auto pixel_pos = Metrics::MetersToPixels(ball_body.Position());
//    circle.setPosition(pixel_pos.X, pixel_pos.Y);
//
//    sf::Color color;
//
//    if (i == 0) {
//      const auto inv_f_percentage =
//          std::abs(force_percentage_ - 1.f);
//      color = sf::Color(255, 255 * inv_f_percentage, 255 * inv_f_percentage);
//    } else if (i == 1) {
//      color = sf::Color::Yellow;
//    } else {
//      color = i % 2 == 0 ? sf::Color::Blue : sf::Color::Red;
//    }
//
//    circle.setFillColor(color);
//    render_target_->draw(circle);
//    i++;
//  }
//}
//
//void GameGraphicsManager::DrawWalls() noexcept {
//  for (std::int16_t i = 0; i < 4; i++) {
//    const auto& col = world_.GetCollider(wall_col_refs_[i]);
//    const auto rect_col =
//        Metrics::MetersToPixels(std::get<Math::RectangleF>(col.Shape()).Size());
//    sf::RectangleShape rect(sf::Vector2f(rect_col.X, rect_col.Y));
//
//    rect.setOrigin(rect_col.X * 0.5f, rect_col.Y * 0.5f);
//
//    auto& body = world_.GetBody(wall_body_refs_[i]);
//    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
//    rect.setPosition(pixel_pos.X, pixel_pos.Y);
//
//    wall_color_ = is_player_turn_ ? sf::Color::Green : sf::Color::Magenta;
//    rect.setFillColor(sf::Color(102, 51, 0));
//
//    render_target_->draw(rect);
//  }
//}
//
//void GameGraphicsManager::DrawHoles() noexcept {
//  for (std::int16_t i = 0; i < 6; i++) {
//    auto& col = world_.GetCollider(hole_col_refs_[i]);
//    const auto radius = Metrics::MetersToPixels(
//        std::get<Math::CircleF>(col.Shape()).Radius() * 2.f);
//
//    sf::CircleShape circle(radius);
//    circle.setOrigin(radius, radius);
//
//    auto& body = world_.GetBody(hole_body_refs_[i]);
//    const auto pixel_pos = Metrics::MetersToPixels(body.Position());
//    circle.setPosition(pixel_pos.X, pixel_pos.Y);
//
//    circle.setFillColor(sf::Color::Black);
//    render_target_->draw(circle);
//  }
//}
//
//void GameGraphicsManager::DrawUi() const noexcept {
//  sf::FloatRect text_bounds{};
//
//  sf::Text score_txt(std::to_string(score_), font_);
//  score_txt.setPosition(150, 50);
//  text_bounds = score_txt.getLocalBounds();
//  score_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
//  render_target_->draw(score_txt);
//
//  sf::Text opp_score_txt(std::to_string(opponent_score_), font_);
//  opp_score_txt.setPosition(window_size_.X - 150, 50);
//  text_bounds = opp_score_txt.getLocalBounds();
//  opp_score_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
//  render_target_->draw(opp_score_txt);
//
//  const auto turn_string = is_player_turn_ ? "Your turn" : "Opponent's turn";
//  sf::Text turn_txt(turn_string, font_);
//  turn_txt.setPosition(window_size_.X * 0.5f, 50.f);
//  text_bounds = turn_txt.getLocalBounds();
//  turn_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
//  render_target_->draw(turn_txt);
//
//  if (is_game_finished_) {
//    const std::string end_game_str = has_win_ ? "Victory !" : "Defeat !";
//    sf::Text end_game_txt(end_game_str, font_);
//    end_game_txt.setPosition(window_size_.X * 0.5f, window_size_.Y * 0.5f);
//    text_bounds = end_game_txt.getLocalBounds();
//    end_game_txt.setOrigin(text_bounds.width * 0.5f, text_bounds.height * 0.5f);
//    render_target_->draw(end_game_txt);
//  }
//}