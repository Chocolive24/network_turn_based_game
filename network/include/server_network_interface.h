#pragma once

#include "packet.h"

#include <functional>

class ServerNetworkInterface {
 public:
  ServerNetworkInterface() noexcept = default;
  ServerNetworkInterface(ServerNetworkInterface&& other) noexcept =
      default;
  ServerNetworkInterface& operator=(
      ServerNetworkInterface&& other) noexcept = default;
  ServerNetworkInterface(
      const ServerNetworkInterface& other) noexcept = default;
  ServerNetworkInterface& operator=(
      const ServerNetworkInterface& other) = default;
  virtual ~ServerNetworkInterface() noexcept = default;

  [[nodiscard]] virtual bool WaitForNetworkEvent(float timeout) noexcept = 0;
  [[nodiscard]] virtual bool AcceptNewConnection() noexcept = 0;
  virtual void SendPacket(sf::Packet* packet, ClientPort client_id) noexcept = 0;
  virtual void PollClientPackets() noexcept = 0;

  void RegisterPacketReceivedCallback(
      const std::function<void(ClientPacket* packet_data)>& callback) {
    packet_received_callback_ = callback;
  }

protected:
  std::function<void(ClientPacket* packet_data)> packet_received_callback_;
};