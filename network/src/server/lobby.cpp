#include "server/lobby.h"

bool Lobby::IsComplete() const noexcept {
  return client_data_1.port > 0 && client_data_2.port > 0;
}

void Lobby::AddClient(const ClientData& client_data) noexcept {
  if (client_data_1.port == 0) {
    client_data_1.port = client_data.port;
    client_data_1.username = client_data.username;
  }
  else {
    client_data_2.port = client_data.port;
    client_data_2.username = client_data.username;
  }
}

void Lobby::Clear() noexcept {
  client_data_1 = ClientData{};
  client_data_2 = ClientData{};
}
