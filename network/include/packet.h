#pragma once

#include <cstddef>

class Packet {
public:
  Packet(void* data, const std::size_t& size) noexcept;

  void* data = nullptr;
  std::size_t size = 0;
};