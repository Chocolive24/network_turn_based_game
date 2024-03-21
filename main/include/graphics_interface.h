/**
 * \headerfile graphics_interface.h
 * This header defines the GraphicsInterface class which is an interface for drawing
 * in a render target.
 * \author Olivier Pachoud
 */

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

/**
 * \brief GraphicsInterface is an interface for drawing in a render target.
 */
class GraphicsInterface {
public:
  GraphicsInterface() noexcept = default;
  GraphicsInterface(GraphicsInterface&& other) noexcept = default;
  GraphicsInterface& operator=(GraphicsInterface&& other) noexcept = default;
  GraphicsInterface(const GraphicsInterface& other) noexcept = default;
  GraphicsInterface& operator=(const GraphicsInterface& other) noexcept = default;
  virtual ~GraphicsInterface() noexcept = default;

  virtual void Init(sf::RenderTarget* render_target) noexcept = 0;
  virtual void Draw() noexcept = 0;
};