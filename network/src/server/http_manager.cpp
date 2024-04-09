#include "server/http_manager.h"

#include <iostream>
#include <string_view>

void HttpManager::RegisterHostAndPort(const std::string_view host,
                                      Port port) noexcept {
  http_.setHost(host.data(), port);
}

std::string HttpManager::Get(const std::string_view uri) noexcept {
  const sf::Http::Request request(uri.data(), sf::Http::Request::Get);
  const sf::Http::Response response = http_.sendRequest(request);

  if (response.getStatus() == sf::Http::Response::Ok) {
    return response.getBody();
  }

  if (response.getStatus() == sf::Http::Response::ConnectionFailed) {
    return "offline";
  }

  std::cerr << "request failed" << '\n';
  return "";
}

void HttpManager::Post(const std::string_view uri,
                       const std::string_view json_body) noexcept {
  sf::Http::Request request(uri.data(), sf::Http::Request::Post, 
                     json_body.data());
  request.setField("Content-Type", "application/json");

  const sf::Http::Response response = http_.sendRequest(request);

  if (response.getStatus() == sf::Http::Response::Ok) {
    std::cout << "Posted : " << response.getBody() << '\n';
  }
  else {
    std::cerr << "Post request failed. \n";
  }
}
