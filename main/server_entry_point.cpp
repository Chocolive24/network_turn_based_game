#include "server.h"

int main() {
  Server server;

  if (server.Init(PORT) == ReturnStatus::kFailure) {
    return EXIT_FAILURE;
  }

  server.Run();

  return EXIT_SUCCESS;
}