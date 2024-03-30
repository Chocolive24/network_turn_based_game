#include "client/client_identifier.h"

#include <string>
#include <fstream>
#include <iostream>

void ClientIdentifier::PerformIdentification(
    const std::function<void(std::string_view username)>& callback) noexcept {
  identification_callback_ = callback;

  return;

  // Open the user_data.txt file to check if the client already have a username.
  // ---------------------------------------------------------------------------
  std::ifstream file("data/user_data.txt");

  if (!file.is_open()) {
    return;
  }

  std::getline(file, username_);

  if (username_.empty()) {
    return;
  }

  if (username_ != kNotRegisteredMessage_) {
    if (identification_callback_) {
      std::cout << "username = " << username_ << '\n';
      identification_callback_(username_);
    }
  }
  else {
    username_.clear();
  }
}

void ClientIdentifier::OnTextEntered(const sf::Event::TextEvent& text_event) noexcept {
  switch (text_event.unicode) {
    case kEnterKeyCode_:
      WriteUsernameInFile();
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

void ClientIdentifier::WriteUsernameInFile() const noexcept {
  std::ofstream outputFile("data/user_data.txt");

  if (outputFile.is_open()) {
    outputFile << username_;
    outputFile.flush();

    outputFile.close();
    std::cout << "Successfully wrote " << username_ << " in the file.\n";
  }
  else {
    std::cout << "Unable to open file for writing.\n";
  }
}