#include <iostream>
#include <SFML/Network/Http.hpp>

#include "server.h"
#include "server_network_manager.h"

int main() {

	sf::Http http("127.0.0.1", 8000);
  const sf::Http::Request get_request("/players", sf::Http::Request::Get);

  sf::Http::Response response = http.sendRequest(get_request);

    // check the status
  if (response.getStatus() == sf::Http::Response::Ok) {
    // check the contents of the response
    std::cout << response.getBody() << '\n';
  } else {
    std::cout << "request failed" << '\n';
  }

  // Prepare the JSON payload
  std::string_view jsonBody = R"({"name": "Olivier"})";

  // Prepare the request
  sf::Http::Request request("/players", sf::Http::Request::Post);
  request.setBody(jsonBody.data());
  request.setField("Content-Type", "application/json");

  // Send the request
  response = http.sendRequest(request);

  // check the status
  if (response.getStatus() == sf::Http::Response::Ok) {
    // check the contents of the response
    std::cout << response.getBody() << '\n';
  }
  else {
    std::cout << "request failed" << '\n';
  }

  //ServerNetworkManager server_network_manager{};
  //if (server_network_manager.ListenToPort(PORT) == ReturnStatus::kFailure) {
  //  return EXIT_FAILURE;
  //}

  //Server server(&server_network_manager);
  //server.Run();

  return EXIT_SUCCESS;
}
