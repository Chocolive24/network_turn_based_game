#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "server.h"

// Mock ServerNetworkInterface
class MockServerNetworkInterface final : public ServerNetworkInterface {
 public:
  ~MockServerNetworkInterface() noexcept override = default;
  void SendPacket(sf::Packet* packet, ClientPort client_id) noexcept override{}
  void PollEvents() noexcept override{}
};

