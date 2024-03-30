#pragma once

#include <functional>
#include <SFML/Window/Event.hpp>

/**
 * \brief ClientIdentifier is a class that open the user_data.txt file to check if
 * the client has already a username.
 * If not, it asks the user the enter a username and writes it in the file.
 */
class ClientIdentifier {
public:
  explicit ClientIdentifier() noexcept = default;

  void RegisterIdentificationCallback(
      const std::function<void(std::string_view username)>& callback) noexcept;
  void OnTextEntered(const sf::Event::TextEvent& text_event) noexcept;

  [[nodiscard]] std::string_view username() const noexcept { return username_; }

 private:

  std::function<void(std::string_view username)> identification_callback_{};
  std::string username_{};
  static constexpr std::int8_t kEnterKeyCode_ = 13;
  static constexpr std::int8_t kSpaceKeyCode_ = 32;
  static constexpr std::int8_t kBackspaceKeyCode_ = 8;
};