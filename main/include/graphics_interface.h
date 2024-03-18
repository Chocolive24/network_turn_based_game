/**
 * \headerfile graphics_interface.h
 * This header defines the DrawInterface class which is an interface to display
 * data on a window.
 * \author Olivier Pachoud
 */

#pragma once

/**
 * \brief GraphicsInterface is a class which is an interface to display
 * data on a window.
 */
class DrawInterface {
public:
  virtual ~DrawInterface() noexcept = default;
  virtual void Draw() noexcept = 0;
};