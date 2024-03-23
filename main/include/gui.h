#pragma once

#include "button.h"

#include <SFML/Graphics/Drawable.hpp>

#include <vector>

class Gui : public sf::Drawable {
public:
  Gui() noexcept = default;
  Gui(Gui&& other) noexcept = default;
  Gui& operator=(Gui&& other) noexcept = default;
  Gui(const Gui& other) noexcept = default;
  Gui& operator=(const Gui& other) noexcept = default;
  ~Gui() noexcept override = default;

  virtual void OnDraw(sf::RenderTarget& target,
                      sf::RenderStates states) const noexcept = 0;

protected:
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

class EntryPointGui final : public Gui {
public:
  ~EntryPointGui() noexcept override = default;

private:
  void OnDraw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override;
};