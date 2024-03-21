#include "client_application.h"

int main() {
  Client client;
  if (client.ConnectToServer(HOST_NAME, PORT, false)
      == ReturnStatus::kFailure){
    return EXIT_FAILURE;
  }

  GameGraphicsManager game_graphics_manager;

  ClientApplication app(&client, &game_graphics_manager);
  const auto app_status = app.Run();

  return app_status == ReturnStatus::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}