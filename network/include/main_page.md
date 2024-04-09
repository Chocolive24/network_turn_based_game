\mainpage Welcome !

## Presentation

Welcome to the documentation of my network code for my turn-based game. <br>
This project was carried out as part of an exercise for SAE Institute Geneva during the Computer Network module of the Game Programming course.
 
I've created this little API based on TCP sockets from the SFML-Network library.<br>
My API provides two network managers, one for the client and one for the server (ClientNetworkManager & ServerNetworkManager), inheriting interfaces so they can be easily mocked. <br>
The same principle applies to the HttpManager class, which enables HTTP GET and POST requests.

This page contains examples and explanations of the various API classes. Feel free to have a look before downloading my project.

## Installation

To start using my network code, I encourage you to build my game project to see how it's implemented and used in the game code, and also to see the CMake environment variables: Here the steps to build my project: 

Requierements :
- Git installed
- CMake installed
- Vcpkg installed
- Visual Studio 17 2022 installed

Build steps:
1. Git clone my turn-based game repository: https://github.com/Chocolive24/network_turn_based_game.git
2. Go to the project root and run this cmake command (or use CMake GUI): 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cmake -S . -B <build_directory_name> -DCMAKE_BUILD_TYPE=Release -DPORT="<a port number greater than 1024>"-DHOST_NAME="<an IP adress>"

-DCMAKE_TOOLCHAIN_FILE=<path_to_your_vcpkg_directory>/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
3. Then run this command: 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cmake --build <build_directory_name>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
4. Go to the build directory and open the turn_based_game.sln solution file.

If you want to reuse my network classes, just copy the network directoy and link it to your project.

## Packet types

The basis of my network code is an enum called PacketType, 
which differentiates the different packets and allows correct interaction with each of them.

PacketType:

- kNone 
- kClientIdentification,
- kJoinLobby,
- KStartGame,
- kNewTurn,
- KCueBallVelocity,
- kEndGame,
- kEloUpdated, 

You can create any type of packet you like for your application.<br>
The first piece of data contained in a packet is always a PacketType so that the way in which the rest of the packet's 
data is read can be adapted. <br>
Here is a small example in pseudo-code for sending a packet:
```cpp
sf::Packet my_packet{};
my_packet << PacketType << any_type_of_data;
client_network_interface_->SendPacket(my_packet);
```

And here is a small example in pseudo-code for receiving and decoding a packet:
```cpp
sf::Packet received_packet;
const auto packet_type = client_network_interface_->ReceivePacket(received_packet);
switch (packet_type) {
  case PacketType::kNone:
    std::cerr << "Packet received has no type. \n";
    break;
  case PacketType::kJoinLobby:
  case PacketType::kClientIdentification: 
  case PacketType::kEloUpdated:
  case PacketType::KStartGame:
  case PacketType::kNewTurn:
  case PacketType::KCueBallVelocity:
  case PacketType::kEndGame:
    // Do what whatever you need to do
    break;
  default:
    break;
}
```
These examples don't use the ClientNetworkManager class directly but rather a pointer to a ClientNetworkInterface for more flexibility and to have the ability to mock up the interface (to test your app withotu network for example). <br>
Of course, you can also use the ClientNetworkManager class directly. 

## ClientPacket 

ClientPacket is an object containing data that a client wishes to send
to the server and its remote port. This struct must be used in the server side only to be able to know which client has sent a packet.

## Types

Here's a collection of useful types and typedefs used by the API.

```cpp

enum class ReturnStatus : std::int8_t {
  kSuccess = 0,
  kFailure = 1
};

struct PlayerData {
  std::string username;
  int elo;
};

using Port = std::uint16_t;
```

## ClientNetworkManager 

ClientNetworkManager is an implementation of the ClientNetworkInterface using a sf::TcpSocket to communicates to the server on the network. Here's a short example of how to use this class:

```cpp
#include "client_network_manager.h"

int main() {
  ClientNetworkManager client;
  if (client.ConnectToServer(HOST_NAME, PORT, false)
      == ReturnStatus::kFailure){
    std::cerr << "Could not connect to server.\n";
    return EXIT_FAILURE;
  }

  std::cout << "Connected to the server.\n";

  // Create a join lobby packet and send it to the server.
  sf::Packet join_lobby_packet;
  join_lobby_packet << PacketType::kJoinLobby << any_data_you_want_to_send;
  client.SendPacket(join_lobby_packet);

  bool is_running = true;

  while(is_running) {
    // Check for received packets.
    sf::Packet received_packet;
    const auto packet_type = client.ReceivePacket(received_packet);
    switch (packet_type) {
        case PacketType::kNone:
            std::cerr << "Packet received has no type. \n";
            break;
        case PacketType::kJoinLobby:
            std::cout << "Client successfuly joinded a lobby.\n";
            break;
        case PacketType::kClientIdentification:
        case PacketType::kEloUpdated:
        case PacketType::KStartGame:
        case PacketType::kNewTurn:
        case PacketType::KCueBallVelocity:
            // TODO: Handle packet receive as you wish.
            break;
        case PacketType::kEndGame:
            is_running = false;
            break;
        default:
            break;  
    }
  }

  client.DisconnectFromServer();

  return EXIT_SUCCESS;
}
```

## ServerNetworkManager

ServerNetworkManager is an implementation of the ServerNetworkInterface using a sf::TcpSocket per client with a sf::SocketSelector and a sf::TcpListener to handle communications between all clients connected to the server. The client's socket are stored in an array of unique_ptr of sf::TcpSocket. <br>
To receive network events from customers, you need to create two functions and give them as callbacks to the ServerNetworkManager. The first callback occurs when a client has sent a packet. <br>
The second callback occurs when a client disconnects from the server.<br>
The PollEvents() method handle the client connections and detects which client send a packet based on its port number.<br>
Here's a short example of how to use this class:

```cpp
#include "server_network_manager.h"

#include <iostream>

// Function callbacks declaration.
// ===============================
void OnPacketReceived(ClientPacket* client_packet) noexcept;
void OnClientDisconnection(const Port client_port) noexcept;

static ServerNetworkManager server_network_manager{};

int main() {
    if (server_network_manager.ListenToPort(PORT) == ReturnStatus::kFailure) {
        std::cerr << "Could not listen to port: " << PORT << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Server is listening to port: " << PORT << '\n';

    // Register callbacks using lambdas.
    // ===================
    server_network_manager.RegisterPacketReceivedCallback(
        [](ClientPacket* client_packet) {
        OnPacketReceived(client_packet);
    }
    );

    server_network_manager.RegisterClientDisconnectionCallback(
        [](const Port client_port) {
        OnClientDisconnection(client_port);
    }
    );

    while (true){
        server_network_manager.PollEvents();
    }

    return EXIT_SUCCES;
}

void OnPacketReceived(ClientPacket* client_packet) noexcept {
  PacketType packet_type = PacketType::kNone;
  client_packet->data >> packet_type;

  switch (packet_type) {
    case PacketType::kNone:
      std::cerr << "Packet received has no type. \n";
      break;
    case PacketType::KNotReady:
    case PacketType::KStartGame:
    case PacketType::kEloUpdated:
    case PacketType::kJoinLobby:
    case PacketType::kNewTurn:
    case PacketType::KCueBallVelocity: 
    case PacketType::kClientIdentification:
    case PacketType::kEndGame: 
      // TODO: Handle packet receive as you wish. 
      // Here we simply send back the packet to the client like an echo server.
      server_network_manager.SendPacket(client_packet->data, client_packet->client_port)
      break;
    default:
      break;
  }
}

void OnClientDisconnection(const Port client_port) noexcept {
  std::cout << "Client on port: " << client_port << " has disconnected from server.\n";
}

```

## HttpManager

HttpManager is an implementation of the HttpInterface which enable to send Http GET and POST requests with a string that can be a json body using a sf::Http object. Here's a short example of how to use this class:

```cpp
#include "http_manager.h"

#include <iostream>

int main() {
    HttpManager http_manager{};

    // Bind http_manager to localhost.
    http_manager.RegisterHostAndPort("127.0.0.1", 8000);

    // Replace the uri by the one of your HTTP page.
    const std::string uri = "/hello";
    const std::string response = http_manager.Get(uri);

    if (response.empty()){
        std::cerr << "Could not Get an answer.\n";
    }
    else {
        std::cout << response << '\n';
    }

    std::string uri = "/hello/message_place";
    const std::string message_body = "My message";
    http_manager.Post(uri, message_body);

    return EXIT_SUCCES;
}
```

## Lobby

The Lobby class provides a simple, naive way of differentiating between the different clients connected to the server.<br>
It uses a struct called ClientData containing a client's username and port number to simplify packet communications between two players in a game. <br>

The class provides three methods:
- IsComplete() 
  Returns true if the lobby is complete.
- AddClient(const ClientData& client_data)
  Adds a client to the lobby via the ClientData type.
- Clear()
  Removes all clients from the lobby. The lobby is now ready to be reused.
