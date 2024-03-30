#include "client/client_identifier.h"

#include <string>

void ClientIdentifier::RegisterIdentificationCallback(
    const std::function<void(std::string_view username)>& callback) noexcept {
  identification_callback_ = callback;
}

void ClientIdentifier::OnTextEntered(const sf::Event::TextEvent& text_event) noexcept {
  switch (text_event.unicode) {
    case kEnterKeyCode_:
      if (identification_callback_ != nullptr)
        identification_callback_(username_);
      break;
    case kBackspaceKeyCode_: 
      if (!username_.empty()) {
        username_.pop_back();
      }
      break;
    case kSpaceKeyCode_: 
      username_ += '_';
      break;
    default:
      if (text_event.unicode > kSpaceKeyCode_ && text_event.unicode < 122) {
        username_ += static_cast<char>(text_event.unicode);
      }
      break;
  }
}