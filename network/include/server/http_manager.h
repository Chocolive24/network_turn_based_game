#pragma once

#include "http_interface.h"

/**
 * \brief HttpManager is an implementation of the HttpInterface which enable to
 * send Http GET and POST requests with a json body using a sf::Http object.
 */
class HttpManager final : public HttpInterface {
public:
  constexpr explicit HttpManager() noexcept = default;
  constexpr HttpManager(HttpManager&& other) noexcept = delete;
  constexpr HttpManager& operator=(HttpManager&& other) noexcept = delete;
  constexpr HttpManager(const HttpManager& other) noexcept = delete;
  constexpr HttpManager& operator=(const HttpManager& other) noexcept = delete;
  ~HttpManager() noexcept override = default;

  void RegisterHostAndPort(std::string_view host, Port port) noexcept override;
  [[nodiscard]] std::string Get(std::string_view uri) noexcept override;
  void Post(std::string_view uri, std::string_view json_body) noexcept override;

  static constexpr char kOfflineMessage[8] = "offline";

private:
  sf::Http http_{};
};