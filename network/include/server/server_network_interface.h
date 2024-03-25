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

  virtual void SendPacket(sf::Packet* packet, Port client_id) noexcept = 0;

  /**
   * \brief PollEvents is a pure virtual method which must poll the network events
   * such as client connections/disconnections and client packets receiving.
   * \note This method has to be overriden.
   */
  virtual void PollEvents() noexcept = 0;

  void RegisterPacketReceivedCallback(
      const std::function<void(ClientPacket* client_packet)>& callback) {
    packet_received_callback_ = callback;
  }

  void RegisterClientDisconnectionCallback(
      const std::function<void(Port client_port)>& callback) {
    disconnect_callback_ = callback;
  }

protected:
  std::function<void(ClientPacket* packet_data)> packet_received_callback_;
  std::function<void(Port client_port)> disconnect_callback_;
};