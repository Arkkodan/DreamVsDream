#ifndef NETWORK_H_INCLUDED
#define NETWORK_H_INCLUDED

#include "globals.h"

#include "player.h"

namespace net {
	#define DEFAULT_PORT 39300 //"ubo" in base 36
	#define DEFAULT_IP 0x0100007F //local loopback, 127.0.0.1

	#define INPUT_SEND_COUNT 8

	enum {
		MODE_NONE,
		MODE_SERVER,
		MODE_CLIENT,
	};

	#define NET_VERSION 2

	struct NetHeader {
	#define NETF_SYN (1<<0)
	#define NETF_ACK (1<<1)
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
	#define socklen_t int
#endif
	extern volatile bool enabled;
	extern volatile bool running;
	extern volatile bool connected;

	extern int inputDelay;
	extern volatile uint32_t frame;

	void* run(void*);
	void terminate();

	void start(uint32_t _ip, uint16_t _port);
	void stop();

	int send(const void* data, size_t size);
	//int sendRaw(const NetHeader* header, const void* data, size_t size);
	int recv(void* data, size_t size);
	//int recvRaw(NetHeader* header, void* data, size_t size);

	game::Player* getMyPlayer();
	game::Player* getYourPlayer();

	void refresh();

	void init(int input_delay);
	void deinit();
}

#endif // NETWORK_H_INCLUDED
