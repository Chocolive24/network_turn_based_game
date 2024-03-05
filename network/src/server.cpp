#include "server.h"

ReturnStatus Server::Init(const unsigned short port) noexcept {
  if (listener_.listen(port) != sf::Socket::Done) {
    std::cerr << "Could not listen to port.\n";
    return ReturnStatus::kFailure;
  }

  std::cout << "Server is listening to port : " << port << '\n';

  socket_selector_.add(listener_);

  return ReturnStatus::kSuccess;
}

void Server::RunServer() noexcept {
  while (true) {
    // Make the selector wait for data on any socket.
    if (socket_selector_.wait(sf::seconds(1.f))) {
      // Test the listener
      if (socket_selector_.isReady(listener_)) {
        // The listener is ready: there is a pending connection.
        // Accept a new connection only if there is not already the maximum
        // number of clients.
        if (client_index_ < kMaxClientCount_) {
          if (listener_.accept(clients_[client_index_]) != sf::Socket::Done) {
            std::cerr << "Could not accept client.\n";
          }

          clients_[client_index_].setBlocking(true);
          socket_selector_.add(clients_[client_index_]);

          std::cout << "Client " << clients_[client_index_].getRemoteAddress()
                    << ':' << clients_[client_index_].getRemotePort()
                    << " is connected. \n";
          client_index_++;
        }
      }

      // Check for data from clients.
      // ----------------------------
      for (auto& client : clients_) {
        if (socket_selector_.isReady(client)) {
          sf::Vector2i circle_pos{};
          sf::Packet packet_received;
          if (client.receive(packet_received) != sf::Socket::Done) {
            std::cerr << "Could not receive packet from client.\n";
          }
          else {
            if (!(packet_received >> circle_pos.x >> circle_pos.y)) {
              std::cerr << "Could not extract data from client's packet.\n";
            }
          }

          // Send received data to other clients
          for (auto& other_client : clients_) {
            if (&other_client != &client) {
              sf::Packet packet;
              if (!(packet << circle_pos.x << circle_pos.y)) {
                std::cerr << "Could not insert data in packet.\n";
              }
              else {
                if (other_client.send(packet) != sf::Socket::Done) {
                  std::cerr << "Could not send data to other client.\n";
                }
              }
            }
          }
        }
      }
    } else {
      // Handle other server tasks or timeout operations here.
      std::cout << "Waiting for activity...\n";
    }
  }
}
