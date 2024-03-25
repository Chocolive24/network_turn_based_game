#pragma once

#include <SFML/Network/Http.hpp>

class HttpInterface {
public:
  virtual ~HttpInterface() noexcept = default;
  virtual void RegisterHostAndPort(std::string_view host,
                                   unsigned short port) noexcept = 0;
  virtual std::string Get(std::string_view uri, sf::Http::Request::Method method) noexcept = 0;
  virtual void Post(std::string_view uri, sf::Http::Request::Method method,
                           std::string_view json_body) noexcept = 0;
};