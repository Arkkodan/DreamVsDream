#ifdef _WIN32
#include <winsock2.h>
#define close(x) closesocket(x)
#define neterrno WSAGetLastError()
#else
#define neterrno errno
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#endif

#include "network.h"
#include "error.h"
#include "scene/scene.h"
#include "thread.h"

#include "sys.h"


namespace net {
	enum {
		NETSTATE_INIT, //Initialize everything
		NETSTATE_LISTEN,
	};

	constexpr auto PACKET_BUFF_SIZE = 256;

	volatile bool enabled = false;
	volatile bool running = false;
	volatile bool connected = false;
	volatile bool halt = false;

#ifndef NO_NETWORK
	Thread netThread;
	Mutex netMutex;

	//File descriptors
	int sock = 0;
#ifndef _WIN32
	fd_set sock_set;
	int sock_pipe[2];
#endif


	socklen_t slen = sizeof(sockaddr_in);
	volatile char mode = MODE_NONE;
	uint32_t cl_ip = 0;
	uint16_t port = DEFAULT_PORT;

	int inputDelay = 0;
	int force_input_delay = 0;
	volatile uint32_t frame = 0;

	sockaddr_in me, you, temp;

	std::string neterror = "";

#define th_return {terminate(); return;}

	void run() {
		//"Global" variables
		int state = NETSTATE_INIT;

		//Nitty gritty networking stuff
		sock = 0;
		memset((char*)&me, 0, sizeof(me));
		memset((char*)&you, 0, sizeof(you));
		memset((char*)&temp, 0, sizeof(temp)); //For receiving data; don't read directly into "you"

		for(;;) {
			switch(state) {
			case NETSTATE_INIT: {
				if(mode == MODE_SERVER) {
					//Create a UDP socket
					if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
						running = false;
						neterror = "Cannot create UDP socket.";
						th_return;
					}

					/*unsigned long mode = 1;
					ioctlsocket(sock, FIONBIO, &mode);*/

					//Initialize "me" inaddr
					me.sin_family = AF_INET;
					me.sin_port = htons(port);
					me.sin_addr.s_addr = htonl(INADDR_ANY);
					if(bind(sock, (sockaddr*)&me, sizeof(me)) == -1) {
						close(sock);
						running = false;
						neterror = "Cannot bind to port " + util::toString(port) + ".";
						th_return;
					}

					//Now wait for a message and perform a handshake which is totally not a ripoff of TCP.

					NetHeader buff;
					if(recv(&buff, sizeof(buff))) {
						if(buff.version != NET_VERSION) {
							buff.version = NET_VERSION;
							send(&buff, sizeof(buff));
							neterror = "Client version differs from server.";
							th_return;
						}

						//If it's a SYN, send a SYN/ACK
						if(buff.flags & NetHeader::NETF_SYN && !(buff.flags & NetHeader::NETF_ACK)) {
							buff.flags = NetHeader::NETF_SYN | NetHeader::NETF_ACK;

							buff.option1 = SceneOptions::optionWins;
							buff.option2 = SceneOptions::optionTime;
							if(send(&buff, sizeof(buff))) {
								unsigned long _timer = sys::getTime(); //Latency calculation
								if(recv(&buff, sizeof(buff))) {
									//Check if its an ACK; if so, connection established; send calculated
									//input delay
									if(buff.flags & NetHeader::NETF_ACK && !(buff.flags & NetHeader::NETF_SYN)) {
										//Calculate the input delay
										float time = (sys::getTime() - _timer) / 1000.0f;
										inputDelay = ceil((time + 0.01) / (2 * sys::SPF));
										if(force_input_delay) {
											inputDelay = force_input_delay;
										}
										connected = true;

										//Send input delay
										send(&inputDelay, 1);
									}
								}
							}
						}
					}
					if(!connected) {
						if(!halt) {
							neterror = "Could not connect to a client.";
						}
						th_return;
					}
					state = NETSTATE_LISTEN;
				} else if(mode == MODE_CLIENT) {
					//Create a UDP socket
					if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
						neterror = "Cannot create UDP socket.";
						th_return;
					}

					//Initialize "you" inaddr
					you.sin_family = AF_INET;
					you.sin_port = htons(port);
					you.sin_addr.s_addr = cl_ip;

					//Send a SYN
					NetHeader buff;
					buff.flags = NetHeader::NETF_SYN;
					buff.version = NET_VERSION;
					if(send(&buff, sizeof(buff))) {
						//Wait for a reply; check for SYN/ACK
						if(recv(&buff, sizeof(buff))) {
							if(buff.version != NET_VERSION) {
								neterror = "Client version differs from server.";
								th_return;
							}

							if(buff.flags & (NetHeader::NETF_SYN | NetHeader::NETF_ACK)) {
								//Save options
								SceneOptions::optionWins = buff.option1;
								SceneOptions::optionTime = buff.option2;

								//Send an ACK. We've got a connection!
								buff.flags = NetHeader::NETF_ACK;
								if(send(&buff, sizeof(buff))) {
									connected = true;

									//Get input delay
									inputDelay = 0;
									recv(&inputDelay, 1);
								}
							}
						}
					}
					if(!connected) {
						if(!halt) {
							neterror = "Could not connect to a server.";
						}
						th_return;
					}
					state = NETSTATE_LISTEN;
				}
			}
			break;

			case NETSTATE_LISTEN: {
				uint8_t buffer[256];
				int size = recv(buffer, 256) - 1;
				uint8_t* buff = buffer + 1;

				if(size + 1) {
					game::Player* p = getYourPlayer();
					//LOCK();
					switch(*buffer) {
					//Update input information
					case PACKET_UPDATE: {
						//Copy every input into the netbuff. There will be redundancy, but that's not too bad
						//considering the problems this will prevent from packet loss.
						uint32_t netframe = *((uint32_t*)buff);
						netMutex.lock();
						for(int i = 0; i < INPUT_SEND_COUNT; i++) {
							if(netframe - i <= 0) {
								break;
							}
							if(i * 16 + 32 >= size) {
								break;
							}
							p->netBuff[p->netBuffCounter].frame = netframe - i;
							p->netBuff[p->netBuffCounter].input = *(uint16_t*)(buff + 32 + i * 16);
							if(++p->netBuffCounter >= game::NETBUFF_SIZE) {
								p->netBuffCounter = 0;
							}
						}
						netMutex.unlock();
					}
					break;

					//Resend input information; this should be very rare, since we're sending multiple inputs in case of packet loss often
					case PACKET_REQUEST: {
						if(size < 4) {
							break;
						}

						//Search for the frame requested and resend it
						uint32_t netframe = *(uint32_t*)buff;
						game::Player* p = getMyPlayer();

                        netMutex.lock();
						for(int i = 0; i < game::NETBUFF_SIZE; i++) {
							if(p->netBuff[i].frame == netframe) {
								//Send this guy back
								uint8_t buffer2[256];
								*buffer2 = PACKET_UPDATE;
								uint8_t* buff2 = buffer2 + 1;

								*(uint32_t*)buff2 = netframe;
								*(uint16_t*)(buff2 + 32) = p->netBuff[i].input;
								send(buffer2, 32 + 16 + 1);
							}
						}
						netMutex.unlock();
					}
					break;
					}
					//UNLOCK();
				} else {
					if(!halt) {
						neterror = "Lost connection to opponent.";
					}
					th_return;
				}
			}
			break;
			}
		}

		//Return cleanly
		th_return;
	}

	void terminate() {
		halt = false;
		running = false;
		connected = false;
		close(sock);
		sock = 0;
#ifndef _WIN32
		close(sock_pipe[0]);
		close(sock_pipe[1]);
#endif
	}
#endif //NO_NETWORK

	void refresh() {
#ifndef NO_NETWORK

		if(connected) {
            netMutex.lock();

			//First, load this input up into our net buffer
			game::Player* p = getMyPlayer();
			game::Player* py = getYourPlayer();
			game::InputBuff* nb = &p->netBuff[p->netBuffCounter];
			nb->input = p->frameInput;
			nb->frame = frame + inputDelay;

			//Construct and send a packet with the last inputDelay inputs
			uint8_t buffer[256];
			buffer[0] = PACKET_UPDATE;
			uint8_t* buff = buffer + 1;
			*(uint32_t*)buff = frame + inputDelay;

			int i = 0;
			int j = p->netBuffCounter;
			for(; i < INPUT_SEND_COUNT; i++) {
				if(!p->netBuff[j].frame) {
					break;
				}
				*(uint16_t*)(buff + 32 + i * 16) = p->netBuff[j].input;
				if(--j < 0) {
					j = game::NETBUFF_SIZE - 1;
				}
			}
			send(buffer, 32 + 16 * i + 1);

			if(++p->netBuffCounter >= game::NETBUFF_SIZE) {
				p->netBuffCounter = 0;
			}

			//Clear out the buffers
			p->frameInput = 0;
			py->frameInput = 0;

			//Get inputs for current frame
			for(i = 0; i < game::NETBUFF_SIZE; i++) {
				if(p->netBuff[i].frame == frame) {
					p->frameInput = p->netBuff[i].input;
				}
			}

			//Wait until we have input for the second character
			bool haveInput = false;
			bool sentRequest = false;
			unsigned int timer;
			unsigned int timeout;
			timer = timeout = sys::getTime();
			while(!haveInput && frame > 20) {
				for(i = 0; i < game::NETBUFF_SIZE; i++) {
					if(py->netBuff[i].frame == frame) {
						haveInput = true;
						py->frameInput = py->netBuff[i].input;
					}
				}
				//If we don't have input, send a request for it every half second.
				if(!haveInput) {
					unsigned int now = sys::getTime();
					if((now - timer) > 500 || !sentRequest) {
						//Pack a simple message and send it
						*buffer = PACKET_REQUEST;
						*(uint32_t*)buff = frame;
						send(buffer, 5);

						sentRequest = true;
						timer = now;
					}
					//sf::sleep(sf::seconds(SPF / 2));

					//If we go past 5 seconds, time out
					if((now - timeout) >= 5 * 1000) {
                        neterror = "Lost connection to opponent.";
						stop();
						Scene::setScene(Scene::SCENE_TITLE);
						break;
					}
				}
			}
			netMutex.unlock();

			frame++;
		} else {
		    if(!neterror.empty()) {
                error::error(neterror);
                neterror = "";
		    }
		}
#endif
	}

	void init(int input_delay_) {
#ifndef NO_NETWORK
		force_input_delay = input_delay_;
#ifdef _WIN32
		//WINDOWS: Initialize winsock
		{
			WSADATA wsaData;
			WORD version = MAKEWORD(2, 0);

			if(WSAStartup(version, &wsaData)) {
				return;
			}

			if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) {
				WSACleanup();
				return;
			}
		}
#endif

		enabled = true;
#endif
	}

	void deinit() {
#ifndef NO_NETWORK
		if(running) {
			stop();
		}
#ifdef _WIN32
		WSACleanup();
#endif
#endif
	}

	void start(uint32_t _ip, uint16_t _port) {
#ifndef NO_NETWORK
		if(!enabled) {
			return;
		}

		cl_ip = _ip;
		port = _port;
		if(!_ip) {
			mode = MODE_SERVER;
		} else {
			mode = MODE_CLIENT;
		}
		frame = 1;

#ifndef _WIN32
		if(pipe(sock_pipe)) {
			neterror = "Could not create network pipe.";
			return;
		}
#endif

		//Create the thread
		netThread = Thread(run);

		running = true;
#endif
	}

	void stop() {
#ifndef NO_NETWORK
		if(!running) {
			return;
		}
		//LOCK();
		halt = true;
#ifdef _WIN32
		close(sock);
#else
        char on = 1;
		write(sock_pipe[1], &on, 1);
#endif
		netThread.join();
#endif
	}

	int send(const void* data, size_t size) {
#ifndef NO_NETWORK
		int sent;

		//Send packet
		if((sent = sendto(sock, (const char*)data, size, 0, (sockaddr*)&you, slen)) <= 0) {
			//free(buff);
			goto handleError;
		}

		return sent;

	handleError:
		neterror = "Could not send packet: " + util::toString(neterrno);
		return 0;
#else
		return 0;
#endif
	}

	int recv(void* data, size_t size) {
#ifndef NO_NETWORK
		int sent;

		//Setup socket set
#ifndef _WIN32
		FD_ZERO(&sock_set);
		FD_SET(sock_pipe[0], &sock_set);
		FD_SET(sock, &sock_set);
#endif

		for(;;) {
#ifndef _WIN32
			//Check for incoming data
			if(select(FD_SETSIZE, &sock_set, nullptr, nullptr, nullptr) == -1) {
				goto handleError;
			}

			//If we're getting data from the pipe, it means that we're being terminated
			if(FD_ISSET(sock_pipe[0], &sock_set)) {
				return 0;
			} else if(FD_ISSET(sock, &sock_set)) {
#endif
				if((sent = recvfrom(sock, (char*)data, size, 0, (sockaddr*)&temp, &slen)) <= 0) {
					goto handleError;
				}
#ifndef _WIN32
			} else {
				goto handleError;
			}
#endif

			//If the client doesn't exist, this is our client for the time being
			if(!you.sin_port) {
				you = temp;
			}

			//Is this the client? If so, break; if not, continue to read data
			if(you.sin_addr.s_addr == temp.sin_addr.s_addr) {
				break;
			}
		}

		return sent;

	handleError:
#ifdef _WIN32
		int _error = neterrno;
		if(_error != WSAEINTR) {
			neterror = "Could not receive packet: " + util::toString(_error);
		}
#else
		neterror = "Could not receive packet: " + util::toString(neterrno);
#endif
		return 0;
#else
		return 0;
#endif
	}

	game::Player* getMyPlayer() {
#ifndef NO_NETWORK
		if(mode == MODE_SERVER) {
			return &SceneFight::madotsuki;
		} else if(mode == MODE_CLIENT) {
			return &SceneFight::poniko;
		}
#endif
		return nullptr;
	}

	game::Player* getYourPlayer() {
#ifndef NO_NETWORK
		if(mode == MODE_SERVER) {
			return &SceneFight::poniko;
		} else if(mode == MODE_CLIENT) {
			return &SceneFight::madotsuki;
		}
#endif
		return nullptr;
	}
}
