#pragma once

#include "types.h"

#include <SFML/Network/Http.hpp>

class HttpInterface {
public:
  constexpr explicit HttpInterface() noexcept = default;
  constexpr HttpInterface(HttpInterface&& other) noexcept = default;
  constexpr HttpInterface& operator=(HttpInterface&& other) noexcept = default;
  constexpr HttpInterface(const HttpInterface& other) noexcept = default;
  constexpr HttpInterface& operator=(const HttpInterface& other) noexcept = default;
  virtual ~HttpInterface() noexcept = default;

  virtual void RegisterHostAndPort(std::string_view host, Port port) noexcept = 0;
  [[nodiscard]] virtual std::string Get(std::string_view uri) noexcept = 0;
  virtual void Post(std::string_view uri,
                    std::string_view json_body) noexcept = 0;
};