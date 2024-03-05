#include <SFML/Network.hpp>
#include <iostream>

#include "SFML/Graphics.hpp"
#include "client.h"
#include "physics_engine/include/World.h"

static constexpr int kSpeed = 5;
static constexpr float kRadius = 50.f;

int main() {
  Client client;
  if(client.ConnectToServer(HOST_NAME, PORT) == 
      ReturnStatus::kFailure) {
    return EXIT_FAILURE;
  }

  // Create window.
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  sf::String window_name = client.remote_address().toString() + " : " +
                           std::to_string(client.remote_port());
  sf::RenderWindow window(sf::VideoMode(1280, 720), window_name,
                          sf::Style::Default, window_settings);
  window.setVerticalSyncEnabled(true);

  sf::Vector2i circle_pos(50.f, 50.f);
  sf::Vector2i other_circle_pos;

  while (window.isOpen()) {
    // Handle events.
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
          case sf::Keyboard::W:
            circle_pos.y -= kSpeed;
            break;
          case sf::Keyboard::A:
            circle_pos.x -= kSpeed;
            break;
          case sf::Keyboard::S:
            circle_pos.y += kSpeed;
            break;
          case sf::Keyboard::D:
            circle_pos.x += kSpeed;
            break;
          default:
            break;
        }
      }
    }

    // Clear window.
    window.clear(sf::Color::Black);

    // Send current position to server.
    sf::Packet packet;
    if (!(packet << circle_pos.x << circle_pos.y)) {
      std::cerr << "Failed to set data into packet." << '\n';
    }
    else {
      client.SendPacket(packet);
    }

    // Receive position from the server.
    sf::Packet received_packet;
    client.ReceivePacket(received_packet);
    if (!(received_packet >> other_circle_pos.x >> other_circle_pos.y)) {
      std::cerr << "Failed to extract data from packet." << '\n';
    }


    // Draw other client's circle
    sf::CircleShape other_circle(kRadius);
    other_circle.setOrigin(kRadius, kRadius);
    other_circle.setPosition(static_cast<sf::Vector2f>(other_circle_pos));
    other_circle.setFillColor(sf::Color::Blue);
    window.draw(other_circle);

    // Draw own circle
    sf::CircleShape circle(kRadius);
    circle.setOrigin(kRadius, kRadius);
    circle.setPosition(static_cast<sf::Vector2f>(circle_pos));
    circle.setFillColor(sf::Color::Red);
    window.draw(circle);

    // Display window.
    window.display();
  }

  return EXIT_SUCCESS;
}
