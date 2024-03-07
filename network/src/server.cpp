#include "server.h"

ReturnStatus Server::Init(const unsigned short port) noexcept {
  if (listener_.listen(port) != sf::Socket::Done) {
    std::cerr << "Could not listen to port : " << port << '.' << '\n';
    return ReturnStatus::kFailure;
  }

  std::cout << "Server is listening to port : " << port << '\n';

  socket_selector_.add(listener_);

  return ReturnStatus::kSuccess;
}

void Server::Run() noexcept {
  while (true) {
    // Make the selector wait for data on any socket.
    if (socket_selector_.wait(sf::seconds(5.f))) {
      // Test the listener
      if (socket_selector_.isReady(listener_)) {
        // The listener is ready: there is a pending connection.
        // Accept a new connection only if there is not already the maximum
        // number of clients.
        if (client_index_ < kMaxClientCount_) {
          AcceptClient();
        }
      }
      else {
        // Check for data from clients.
        // ----------------------------
        CommunicatePacketBetweenClients();
      } // if selector.wait()
    }
    else {
      // Handle other server tasks or timeout operations here.
      std::cout << "Waiting for activity...\n";
    }
  }
}

void Server::AcceptClient() noexcept {
  if (listener_.accept(clients_[client_index_]) != sf::Socket::Done) {
    std::cerr << "Could not accept client.\n";
  }

  clients_[client_index_].setBlocking(true);
  socket_selector_.add(clients_[client_index_]);

  std::cout << "Client " << clients_[client_index_].getRemoteAddress() << ':'
            << clients_[client_index_].getRemotePort() << " is connected. \n";
  client_index_++;
}

void Server::CommunicatePacketBetweenClients() noexcept {
  for (auto& client : clients_) {
    if (socket_selector_.isReady(client)) {
      sf::Packet packet_received;
      if (client.receive(packet_received) != sf::Socket::Done) {
        std::cerr << "Could not receive packet from client.\n";
      }

      // Send received data to other clients
      for (auto& other_client : clients_) {
        if (&other_client != &client) {
          if (other_client.send(packet_received) != sf::Socket::Done) {
            std::cerr << "Could not send data to other client.\n";
          }
        }
      }
    }  // for auto client
  } // else
}
