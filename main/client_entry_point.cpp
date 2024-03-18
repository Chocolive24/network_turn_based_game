#include "client_application.h"

int main() {
  ClientApplication game;
  const auto game_status = game.Run();

  return game_status == ReturnStatus::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}