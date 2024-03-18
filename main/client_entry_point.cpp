#include "client_application.h"

int main() {
  Client client;
  if (client.ConnectToServer(HOST_NAME, PORT, false)
      == ReturnStatus::kFailure){
    return EXIT_FAILURE;
  }

  ClientApplication game(&client);
  const auto game_status = game.Run();

  return game_status == ReturnStatus::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}