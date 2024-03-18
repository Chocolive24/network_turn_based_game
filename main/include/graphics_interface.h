/**
 * \headerfile graphics_interface.h
 * This header defines the GraphicsInterface class which is an interface to display
 * data on a window.
 * \author Olivier Pachoud
 */

#pragma once

/**
 * \brief GraphicsInterface is a class which is an interface to display
 * data on a window.
 */
class GraphicsInterface {
public:
  virtual ~GraphicsInterface() noexcept = 0;
  virtual void Init() noexcept = 0;
  virtual void Draw() noexcept = 0;
};