#include "client_application.h"

int main() {
  ClientNetworkManager client;
  if (client.ConnectToServer(HOST_NAME, PORT, false)
      == ReturnStatus::kFailure){
    return EXIT_FAILURE;
  }

  ClientApplication app(&client);
  const auto app_status = app.Run();

  client.DisconnectFromServer();

  return app_status == ReturnStatus::kSuccess ? EXIT_SUCCESS : EXIT_FAILURE;
}