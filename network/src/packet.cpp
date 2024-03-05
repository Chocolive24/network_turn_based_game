#include "packet.h"

Packet::Packet(void* data, const std::size_t& size) noexcept :
  data(data),
  size(size)
{}