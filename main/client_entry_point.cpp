#include "client.h"
#include "World.h"
#include "Timer.h"
#include "Metrics.h"

#include <SFML/Network.hpp>
#include "SFML/Graphics.hpp"

#include <iostream>

static constexpr int kSpeed = 5;
static constexpr float kPixelRadius = 50.f;
static constexpr float kMeterRadius = 0.5f;

class MyGame final : public PhysicsEngine::ContactListener {
public:
  ~MyGame() override = default;
  void OnTriggerEnter(PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept override;
  void OnTriggerStay(PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept override;
  void OnTriggerExit(PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept override;
  void OnCollisionEnter(PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept override;
  void OnCollisionExit(PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept override;
};

int main() {
  Client client;
  if(client.ConnectToServer(HOST_NAME, PORT) == 
      ReturnStatus::kFailure) {
    return EXIT_FAILURE;
  }

  Timer timer;
  timer.Init();

  // Physics.
  // --------
  PhysicsEngine::World world;
  world.Init(Math::Vec2F::Zero(), 16);
  MyGame game;
  world.SetContactListener(&game);

  std::array<PhysicsEngine::BodyRef, 16> body_refs_{};
  std::array<PhysicsEngine::ColliderRef, 16> collider_refs_{};

  for (int i = 0; i < 2; i++) {
    body_refs_[i] = world.CreateBody();
    collider_refs_[i] = world.CreateCollider(body_refs_[i]);
  }

  auto& cue_ball_body = world.GetBody(body_refs_[0]);
  cue_ball_body.SetBodyType(PhysicsEngine::BodyType::Dynamic);
  cue_ball_body.SetPosition(Math::Vec2F(5.f, -5.f));
  cue_ball_body.SetDamping(0.5f);

  auto& cue_ball_col = world.GetCollider(collider_refs_[0]);
  cue_ball_col.SetShape(Math::CircleF(Math::Vec2F::Zero(), kMeterRadius));
  cue_ball_col.SetFriction(1.f);
  cue_ball_col.SetRestitution(1.f);

  auto& rnd_ball_body = world.GetBody(body_refs_[1]);
  rnd_ball_body.SetBodyType(PhysicsEngine::BodyType::Dynamic);
  rnd_ball_body.SetPosition(Math::Vec2F(6.f, -6.f));
  rnd_ball_body.SetDamping(0.1f);

  auto& rnd_ball_col = world.GetCollider(collider_refs_[1]);
  rnd_ball_col.SetShape(Math::CircleF(Math::Vec2F::Zero(), kMeterRadius));
  rnd_ball_col.SetFriction(1.f);
  rnd_ball_col.SetRestitution(1.f);

  // Create window.
  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = 4;
  sf::String window_name = client.remote_address().toString() + " : " +
                           std::to_string(client.remote_port());
  sf::RenderWindow window(sf::VideoMode(1280, 720), window_name,
                          sf::Style::Default, window_settings);
  window.setVerticalSyncEnabled(true);

  sf::Vector2i other_circle_pos;

  bool is_mouse_pressed = false;
  bool was_mouse_pressed = false;
  bool is_mouse_released = false;

  bool is_charging = false;

  while (window.isOpen()) {
    // Handle events.
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::MouseButtonPressed:
        is_mouse_pressed = true;
        is_mouse_released = false;
        break;
      case sf::Event::MouseButtonReleased:
        is_mouse_released = true;
        is_mouse_pressed = false;
      default:
        break;
      }
    }

    bool is_mouse_just_pressed =
        was_mouse_pressed == false && is_mouse_pressed == true;

    timer.Tick();

    world.Update(timer.DeltaTime());

    const auto mouse_pos =
        Math::Vec2F(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    const auto ball_pos_in_pix = Metrics::MetersToPixels(
        Math::Vec2F(cue_ball_body.Position().X, cue_ball_body.Position().Y));
    const auto distance = mouse_pos - ball_pos_in_pix;

    if (distance.Length<float>() <= kPixelRadius && is_mouse_just_pressed) {
      std::cout << "mouse on circle\n";
      is_charging = true;
    }

    if (is_charging && is_mouse_released) {
      is_charging = false;

      std::cout << Metrics::PixelsToMeters(distance).X << " "
                << Metrics::PixelsToMeters(distance).Y << '\n';

      const auto force = Metrics::PixelsToMeters(distance);

      cue_ball_body.SetVelocity(Math::Vec2F(-force));
    }

    // Clear window.
    window.clear(sf::Color::Black);

    // Send current position to server.
    sf::Packet packet;
    sf::Vector2i sf_pos(cue_ball_body.Position().X, cue_ball_body.Position().Y);
    if (!(packet << sf_pos.x << sf_pos.y)) {
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
    sf::CircleShape other_circle(kPixelRadius);
    other_circle.setOrigin(kPixelRadius, kPixelRadius);
    const auto other_pos = Metrics::MetersToPixels(Math::Vec2F(other_circle_pos.x, other_circle_pos.y));
    other_circle.setPosition(other_pos.X, other_pos.Y);
    other_circle.setFillColor(sf::Color::Blue);
    window.draw(other_circle);

    // Draw own circle
    sf::CircleShape circle(kPixelRadius);
    circle.setOrigin(kPixelRadius, kPixelRadius);
    const auto circle_pixel_pos = Metrics::MetersToPixels(cue_ball_body.Position());
    circle.setPosition(circle_pixel_pos.X, circle_pixel_pos.Y);
    circle.setFillColor(sf::Color::Red);
    window.draw(circle);

    sf::CircleShape rnd_circle(kPixelRadius);
    rnd_circle.setOrigin(kPixelRadius, kPixelRadius);
    const auto rnd_circle_pixel_pos = Metrics::MetersToPixels(rnd_ball_body.Position());
    rnd_circle.setPosition(rnd_circle_pixel_pos.X, rnd_circle_pixel_pos.Y);
    rnd_circle.setFillColor(sf::Color::Red);
    window.draw(rnd_circle);

    // Display window.
    window.display();

    was_mouse_pressed = is_mouse_pressed;
  }

  return EXIT_SUCCESS;
}

void MyGame::OnTriggerEnter(PhysicsEngine::ColliderRef colliderRefA,
                            PhysicsEngine::ColliderRef colliderRefB) noexcept {}

void MyGame::OnTriggerStay(PhysicsEngine::ColliderRef colliderRefA,
                           PhysicsEngine::ColliderRef colliderRefB) noexcept {}

void MyGame::OnTriggerExit(PhysicsEngine::ColliderRef colliderRefA,
                           PhysicsEngine::ColliderRef colliderRefB) noexcept {}

void MyGame::OnCollisionEnter(
    PhysicsEngine::ColliderRef colliderRefA,
    PhysicsEngine::ColliderRef colliderRefB) noexcept {
  std::cout << "collison\n";
}

void MyGame::OnCollisionExit(PhysicsEngine::ColliderRef colliderRefA,
                             PhysicsEngine::ColliderRef colliderRefB) noexcept {
}
