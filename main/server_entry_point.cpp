#include "server.h"

int main() {
  ServerNetworkManager server_network_manager{};
  if (server_network_manager.ListenToPort(PORT) == ReturnStatus::kFailure) {
    return EXIT_FAILURE;
  }

  Server server(&server_network_manager);
  server.Run();

  return EXIT_SUCCESS;
}