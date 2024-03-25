#include "server.h"
#include "http_manager.h"
#include "server_network_manager.h"

int main() {
  ServerNetworkManager server_network_manager{};
  if (server_network_manager.ListenToPort(PORT) == ReturnStatus::kFailure) {
    return EXIT_FAILURE;
  }

  HttpManager http_manager;

  Server server(&server_network_manager, &http_manager);
  server.Run();

  return EXIT_SUCCESS;
}
