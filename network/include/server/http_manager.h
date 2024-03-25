#pragma once

#include "http_interface.h"

struct JsonRequestBody {
  //TODO: pour simplifer la création de body json.
};

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

private:
  sf::Http http_{};
};