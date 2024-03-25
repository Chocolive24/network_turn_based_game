#pragma once

#include "http_interface.h"

struct JsonRequestBody {
  //TODO: pour simplifer la création de body json.
};

class HttpManager final : public HttpInterface {
public:
  ~HttpManager() noexcept override = default;

  void RegisterHostAndPort(std::string_view host, unsigned short port) noexcept override;

  [[nodiscard]] std::string Get(std::string_view uri, 
      sf::Http::Request::Method method) noexcept override;

  void Post(std::string_view uri, sf::Http::Request::Method method, 
            std::string_view json_body) noexcept override;

private:
  sf::Http http_{};
};