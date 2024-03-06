#pragma once

#include "client.h"
#include "World.h"
#include "Timer.h"
#include "Metrics.h"

#include <SFML/Network.hpp>
#include "SFML/Graphics.hpp"

#include <iostream>

class Game {
public:

  void Run() noexcept;

 private:
  Timer timer_{};
  PhysicsEngine::World world_{};

  sf::RenderWindow window_{};

  void Init() noexcept;
};