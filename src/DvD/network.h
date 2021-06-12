#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "player.h"

#include <cstdint>

namespace net {
  constexpr auto DEFAULT_PORT = 39300;    //"ubo" in base 36
  constexpr auto DEFAULT_IP = 0x0100007F; // local loopback, 127.0.0.1

  constexpr auto INPUT_SEND_COUNT = 8;

  enum {
    MODE_NONE,
    MODE_SERVER,
    MODE_CLIENT,
  };

  constexpr auto NET_VERSION = 2;

  /// @brief Basic data packet format
  struct NetHeader {
    static constexpr auto NETF_SYN = 1 << 0;
    static constexpr auto NETF_ACK = 1 << 1;
    uint8_t flags;
    uint8_t version;
    uint8_t option1;
    uint8_t option2;
    uint8_t option3;
    uint8_t option4;
  };

  enum {
    PACKET_UPDATE,
    PACKET_REQUEST,
  };

#ifdef _WIN32
  typedef int socklen_t;
#endif
  extern volatile bool enabled;
  extern volatile bool running;
  extern volatile bool connected;

  extern int inputDelay;
  extern volatile uint32_t frame;

  /// @brief Unused
  /// @details There exists a used overload of this function
  void *run(void *);
  /// @brief Close the connection cleanly
  void terminate();

  /// @brief Start the network
  void start(uint32_t _ip, uint16_t _port);
  /// @brief Stop the network
  void stop();

  /// @brief Send data
  int send(const void *data, size_t size);
  // int sendRaw(const NetHeader* header, const void* data, size_t size);

  /// @brief Receive data
  int recv(void *data, size_t size);
  // int recvRaw(NetHeader* header, void* data, size_t size);

  /// @brief Get local player
  game::Player *getMyPlayer();
  /// @brief Get opponent to local player
  game::Player *getYourPlayer();

  /// @brief Do one frame
  void refresh();

  /// @brief Initialize network framework
  void init(int input_delay);
  /// @brief Deinitialize network framework
  void deinit();
} // namespace net

#endif // NETWORK_H_INCLUDED
