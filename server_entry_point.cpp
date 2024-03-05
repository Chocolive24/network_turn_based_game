#include "server.h"

int main() {
  Server server;

  if (server.Init(PORT) == ReturnStatus::kFailure) {
    return EXIT_FAILURE;
  }

  server.RunServer();

  return EXIT_SUCCESS;
}