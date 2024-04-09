#pragma once

#include "types.h"

/**
 * \brief ClientData is a struct which contains the username of a client and
 * its port number.
 * It is used by the Lobby class to provide an easy way to differentiate the
 * clients in a game and facilitate the packet communications between them.
 */
struct ClientData {
  std::string username{};
  Port port = 0;
};

/**
 * \brief Lobby is a class which provides a simple, naive way of differentiating
 * between the different clients connected to the server.
 * It uses the ClientData struct  to simplify packet communications between
 * two players in a game.
 */
class Lobby {
public:
  [[nodiscard]] bool IsComplete() const noexcept;
  void AddClient(const ClientData& client_data) noexcept;
  void Clear() noexcept;

  ClientData client_data_1;
  ClientData client_data_2;
};