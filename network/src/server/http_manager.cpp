#include "server/http_manager.h"

#include <iostream>
#include <string_view>

void HttpManager::RegisterHostAndPort(const std::string_view host,
                                      const unsigned short port) noexcept {
  http_.setHost(host.data(), port);
}

std::string HttpManager::Get(const std::string_view uri,
                      const sf::Http::Request::Method method) noexcept {
  const sf::Http::Request request(uri.data(), method);
  const sf::Http::Response response = http_.sendRequest(request);

  if (response.getStatus() == sf::Http::Response::Ok) {
    return response.getBody();
  }

  std::cerr << "request failed" << '\n';
  return "";
}

void HttpManager::Post(const std::string_view uri,
                       const sf::Http::Request::Method method,
                       const std::string_view json_body) noexcept {
  sf::Http::Request request(uri.data(), method, json_body.data());
  request.setField("Content-Type", "application/json");

  const sf::Http::Response response = http_.sendRequest(request);

  if (response.getStatus() == sf::Http::Response::Ok) {
    std::cout << "Posted : " << response.getBody() << '\n';
  }
  else {
    std::cerr << "Post request failed. \n";
  }
}
