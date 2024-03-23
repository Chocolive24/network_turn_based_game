#pragma once

#include "packet.h"

#include <functional>

/**
 * \brief ServerNetworkInterface is an interface to communicates with multiple
 * clients.
 */
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

  virtual void SendPacket(sf::Packet* packet, ClientPort client_id) noexcept = 0;
  virtual void PollEvents() noexcept = 0;

  void RegisterPacketReceivedCallback(
      const std::function<void(ClientPacket* packet_data)>& callback) {
    packet_received_callback_ = callback;
  }

  void RegisterClientDisconnectionCallback(
      const std::function<void(ClientPort client_port)>& callback) {
    disconnect_callback_ = callback;
  }

protected:
  std::function<void(ClientPacket* packet_data)> packet_received_callback_;
  std::function<void(ClientPort client_port)> disconnect_callback_;
};