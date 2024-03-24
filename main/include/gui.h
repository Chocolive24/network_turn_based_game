#pragma once

#include <functional>

#include "button.h"

#include <SFML/Graphics/Drawable.hpp>

#include <vector>
#include <SFML/Window/Event.hpp>

class Gui : public sf::Drawable {
public:
  Gui() noexcept = default;
  Gui(Gui&& other) noexcept = default;
  Gui& operator=(Gui&& other) noexcept = default;
  Gui(const Gui& other) noexcept = default;
  Gui& operator=(const Gui& other) noexcept = default;
  ~Gui() noexcept override = default;

  void Update(sf::Vector2f mouse_pos) noexcept;

  virtual void OnEvent(const sf::Event& event) noexcept = 0;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override{}

protected:
  virtual void OnUpdate() noexcept = 0;

protected:
  sf::Vector2f mouse_pos_{};
};

class ClientApplication;

class MainMenuGui final : public Gui {
public:
  explicit MainMenuGui(ClientApplication* client_app) noexcept;
  ~MainMenuGui() noexcept override = default;
  
  void OnEvent(const sf::Event& event) noexcept override;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


private:
  void OnUpdate() noexcept override;

  ClientApplication* client_app_ = nullptr;
};