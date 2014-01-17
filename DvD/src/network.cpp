#include <stdarg.h>

#include <pthread.h>

#include "globals.h"

#ifdef _WIN32
#include <winsock2.h>
#define close(x) closesocket(x)
#define neterrno WSAGetLastError()
#define EWOULDBLOCK WSAEWOULDBLOCK
#define ECONNRESET WSAECONNRESET
#else
#define neterrno errno
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#endif

#include "network.h"
#include "error.h"
#include "menu.h"

#define LOCK() pthread_mutex_lock(&netMutex)
#define UNLOCK() pthread_mutex_unlock(&netMutex)
#define WAIT() pthread_join(netThread, NULL)

#define PACKET_BUFF_SIZE 256

enum
{
    NETSTATE_INIT, //Initialize everything
    NETSTATE_LISTEN,
};

extern Player madotsuki;
extern Player poniko;

namespace Network
{
    volatile bool enabled = false;
    volatile bool running = false;
    volatile bool connected = false;

#ifndef NO_NETWORK
    pthread_t netThread;
    //static pthread_mutex_t netMutex;

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

    bool halt = false;

    sockaddr_in me, you, temp;

    char b_neterror[256];
    bool neterror_exists = false;

    void neterror(const char* fsz_, ...)
    {
        //while(neterror_exists)
            //usleep(100);

        va_list _args;
        va_start(_args, fsz_);
        vsprintf((char*)b_neterror, (const char*)fsz_, _args);
        va_end(_args);
        neterror_exists = true;
    }

    void neterrorRefresh()
    {
        if(neterror_exists)
        {
			error(b_neterror);
            neterror_exists = false;
        }
    }

#define th_return {terminate(); return NULL;}

    void* run(void*)
    {
        //"Global" variables
        int state = NETSTATE_INIT;

        //Nitty gritty networking stuff
        sock = 0;
        memset((char *)&me, 0, sizeof(me));
        memset((char *)&you, 0, sizeof(you));
        memset((char *)&temp, 0, sizeof(temp)); //For receiving data; don't read directly into "you"

        for(;;)
        {
            switch(state)
            {
                case NETSTATE_INIT:
                {
                    if(mode == MODE_SERVER)
                    {
                        //Create a UDP socket
                        if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
                        {
                            running = false;
                            neterror("Cannot create UDP socket.");
                            th_return;
                        }

                        /*unsigned long mode = 1;
                        ioctlsocket(sock, FIONBIO, &mode);*/

                        //Initialize "me" inaddr
                        me.sin_family = AF_INET;
                        me.sin_port = htons(port);
                        me.sin_addr.s_addr = htonl(INADDR_ANY);
                        if(bind(sock, (sockaddr*)&me, sizeof(me)) == -1)
                        {
                            close(sock);
                            running = false;
                            neterror("Cannot bind to port %d.", port);
                            th_return;
                        }

                        //Now wait for a message and perform a handshake which is totally not a ripoff of TCP.

                        NetHeader buff;
                        if(recv(&buff, sizeof(buff)))
                        {
                            if(buff.version != NET_VERSION)
                            {
                                buff.version = NET_VERSION;
                                send(&buff, sizeof(buff));
                                neterror("Client version differs from server.");
                                th_return;
                            }

                            if(buff.option3)
                                optionSecretCharacter = true;

                            //If it's a SYN, send a SYN/ACK
                            if(buff.flags & NETF_SYN && !(buff.flags & NETF_ACK))
                            {
                                buff.flags = NETF_SYN | NETF_ACK;

                                buff.option1 = optionWins;
                                buff.option2 = optionTime;
                                buff.option3 = optionSecretCharacter;
                                if(send(&buff, sizeof(buff)))
                                {
                                    unsigned long _timer = OS::getTime(); //Latency calculation
                                    if(recv(&buff, sizeof(buff)))
                                    {
                                        //Check if its an ACK; if so, connection established; send calculated
                                        //input delay
                                        if(buff.flags & NETF_ACK && !(buff.flags & NETF_SYN))
                                        {
                                            //Calculate the input delay
                                            float time = (OS::getTime() - _timer) / 1000.0f;
                                            inputDelay = ceil((time + 0.01) / (2 * SPF));
                                            if(force_input_delay)
                                                inputDelay = force_input_delay;
                                            printf("Input Delay: %d\n", inputDelay);

                                            printf("SERVER: Connection established.\n");
                                            connected = true;

                                            //Send input delay
                                            send(&inputDelay, 1);
                                        }
                                    }
                                }
                            }
                        }
                        if(!connected)
                        {
                            if(!halt) neterror("Could not connect to a client.");
                            th_return;
                        }
                        state = NETSTATE_LISTEN;
                    }
                    else if(mode == MODE_CLIENT)
                    {
                        //Create a UDP socket
                        if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
                        {
                            neterror("Cannot create UDP socket.");
                            th_return;
                        }

                        //Initialize "you" inaddr
                        you.sin_family = AF_INET;
                        you.sin_port = htons(port);
                        you.sin_addr.s_addr = cl_ip;

                        //Send a SYN
                        NetHeader buff;
                        buff.flags = NETF_SYN;
                        buff.version = NET_VERSION;
                        buff.option3 = optionSecretCharacter;
                        if(send(&buff, sizeof(buff)))
                        {
                            //Wait for a reply; check for SYN/ACK
                            if(recv(&buff, sizeof(buff)))
                            {
                                if(buff.version != NET_VERSION)
                                {
                                    neterror("Client version differs from server.");
                                    th_return;
                                }

                                if(buff.flags & (NETF_SYN | NETF_ACK))
                                {
                                    //Save options
                                    optionWins = buff.option1;
                                    optionTime = buff.option2;
                                    if(buff.option3)
                                        optionSecretCharacter = true;

                                    //Send an ACK. We've got a connection!
                                    buff.flags = NETF_ACK;
                                    if(send(&buff, sizeof(buff)))
                                    {
                                        printf("CLIENT: Connection established.\n");
                                        connected = true;

                                        //Get input delay
                                        inputDelay = 0;
                                        recv(&inputDelay, 1);
                                    }
                                }
                            }
                        }
                        if(!connected)
                        {
                            if(!halt)
								neterror("Could not connect to a server.");
                            th_return;
                        }
                        state = NETSTATE_LISTEN;
                    }
                }
                break;

                case NETSTATE_LISTEN:
                {
                    ubyte_t buffer[256];
                    int size = recv(buffer, 256) - 1;
                    ubyte_t* buff = buffer + 1;

                    if(size + 1)
                    {
                        Player* p = getYourPlayer();
                        //LOCK();
                        switch(*buffer)
                        {
                            //Update input information
                            case PACKET_UPDATE:
                            {
                                //Copy every input into the netbuff. There will be redundancy, but that's not too bad
                                //considering the problems this will prevent from packet loss.
                                uint32_t netframe = *((uint32_t*)buff);
                                for(int i = 0; i < INPUT_SEND_COUNT; i++)
                                {
                                    if(netframe - i <= 0) break;
                                    if(i * 16 + 32 >= size) break;
                                    p->netBuff[p->netBuffCounter].frame = netframe - i;
                                    p->netBuff[p->netBuffCounter].input = *(uint16_t*)(buff + 32 + i * 16);
                                    if(++p->netBuffCounter >= NETBUFF_SIZE)
                                        p->netBuffCounter = 0;
                                }
                            }
                            break;

                            //Resend input information; this should be very rare, since we're sending multiple inputs in case of packet loss often
                            case PACKET_REQUEST:
                            {
                                if(size < 4) break;

                                //Search for the frame requested and resend it
                                uint32_t netframe = *(uint32_t*)buff;
                                //printf("Got a request for frame %d.\n", frame);
                                Player* p = getMyPlayer();

                                for(int i = 0; i < NETBUFF_SIZE; i++)
                                {
                                    if(p->netBuff[i].frame == netframe)
                                    {
                                        //Send this guy back
                                        ubyte_t buffer2[256];
                                        *buffer2 = PACKET_UPDATE;
                                        ubyte_t* buff2 = buffer2 + 1;

                                        *(uint32_t*)buff2 = netframe;
                                        *(uint16_t*)(buff2 + 32) = p->netBuff[i].input;
                                        send(buffer2, 32 + 16 + 1);
                                    }
                                }
                            }
                            break;
                        }
                        //UNLOCK();
                    }
                    else
                    {
                        if(!halt) neterror("Lost connection to opponent.");
                        th_return;
                    }
                }
                break;
            }
        }

        //Return cleanly
        th_return;
    }

    void terminate()
    {
        halt = false;
        running = false;
        connected = false;
        close(sock);
        sock = 0;
#ifndef _WIN32
        close(sock_pipe[0]);
        close(sock_pipe[1]);
#endif
        //pthread_mutex_destroy(&netMutex);
    }
#endif //NO_NETWORK

    void refresh()
    {
#ifndef NO_NETWORK
        neterrorRefresh();

        if(connected)
        {
            //First, load this input up into our net buffer
            Player* p = getMyPlayer();
            Player* py = getYourPlayer();
            InputBuff* nb = p->netBuff + p->netBuffCounter;
            nb->input = p->frameInput;
            nb->frame = frame + inputDelay;

            //Construct and send a packet with the last inputDelay inputs
            ubyte_t buffer[256];
            buffer[0] = PACKET_UPDATE;
            ubyte_t* buff = buffer + 1;
            *(uint32_t*)buff = frame + inputDelay;

            int i = 0;
            int j = p->netBuffCounter;
            for(; i < INPUT_SEND_COUNT; i++)
            {
                if(!p->netBuff[j].frame) break;
                *(uint16_t*)(buff + 32 + i * 16) = p->netBuff[j].input;
                if(--j < 0) j = NETBUFF_SIZE - 1;
            }
            send(buffer, 32 + 16 * i + 1);

            if(++p->netBuffCounter >= NETBUFF_SIZE)
                p->netBuffCounter = 0;

            //Clear out the buffers
            p->frameInput = 0;
            py->frameInput = 0;

            //Get inputs for current frame
            for(i = 0; i < NETBUFF_SIZE; i++)
            {
                if(p->netBuff[i].frame == frame)
                {
                    p->frameInput = p->netBuff[i].input;
                }
            }

            //Wait until we have input for the second character
            bool haveInput = false;
            bool sentRequest = false;
            unsigned int timer;
            unsigned int timeout;
            timer = timeout = OS::getTime();
            while(!haveInput && frame > 20)
            {
                for(i = 0; i < NETBUFF_SIZE; i++)
                {
                    if(py->netBuff[i].frame == frame)
                    {
                        haveInput = true;
                        py->frameInput = py->netBuff[i].input;
                    }
                }
                //If we don't have input, send a request for it every half second.
                if(!haveInput)
                {
                    unsigned int now = OS::getTime();
                    if((now - timer) > 500 || !sentRequest)
                    {
                        //Pack a simple message and send it
                        *buffer = PACKET_REQUEST;
                        *(uint32_t*)buff = frame;
                        send(buffer, 5);

                        sentRequest = true;
                        timer = now;
                    }
                    //sf::sleep(sf::seconds(SPF / 2));

                    //If we go past 5 seconds, time out
                    if((now - timeout) >= 5 * 1000)
                    {
                        stop();
                        Menu::setMenu(MENU_TITLE);
                        break;
                    }
                }
            }

            frame++;
        }
#endif
    }

    void init(int input_delay_)
    {
#ifndef NO_NETWORK
        force_input_delay = input_delay_;
    #ifdef _WIN32
        //WINDOWS: Initialize winsock
        {
            WSADATA wsaData;
            WORD version = MAKEWORD(2, 0);

            if(WSAStartup(version, &wsaData))
                return;

            if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0)
            {
                WSACleanup();
                return;
            }
        }
    #endif

        //Mutex
        /*if(pthread_mutex_init(&netMutex, NULL))
        {
            neterror("Could not initialize mutex.");
            return;
        }*/

        enabled = true;
#endif
    }

    void deinit()
    {
#ifndef NO_NETWORK
        if(running) stop();
    #ifdef _WIN32
        WSACleanup();
    #endif
#endif
    }

    void start(uint32_t _ip, uint16_t _port)
    {
#ifndef NO_NETWORK
        if(!enabled) return;

        cl_ip = _ip;
        port = _port;
        if(!_ip) mode = MODE_SERVER;
        else mode = MODE_CLIENT;
        frame = 1;

#ifndef _WIN32
        if(pipe(sock_pipe))
        {
            neterror("Could not create network pipe.");
            return;
        }
#endif

        //Create the pthread
        if(pthread_create(&netThread, NULL, &run, NULL))
        {
            neterror("Could not start network thread.");
            return;
        }

        running = true;
#endif
    }

    void stop()
    {
#ifndef NO_NETWORK
        if(!running) return;
        //LOCK();
            halt = true;
#ifdef _WIN32
            close(sock);
#else
            write(sock_pipe[1], &halt, 1);
#endif
        //UNLOCK();
        while(running)
        {
            neterrorRefresh();
            //SLEEP(100);
        }
        WAIT();
#endif
    }

    int send(const void* data, size_t size)
    {
#ifndef NO_NETWORK
        int sent;

        //Send packet
        if((sent = sendto(sock, (const char*)data, size, 0, (sockaddr*)&you, slen)) <= 0)
        {
            //free(buff);
            goto handleError;
        }

        return sent;

    handleError:
        neterror("Could not send packet: %d", neterrno);
        return 0;
#else
	return 0;
#endif
    }

    int recv(void* data, size_t size)
    {
#ifndef NO_NETWORK
        int sent;

        //Setup socket set
#ifndef _WIN32
        FD_ZERO(&sock_set);
        FD_SET(sock_pipe[0], &sock_set);
        FD_SET(sock, &sock_set);
#endif

        for(;;)
        {
#ifndef _WIN32
            //Check for incoming data
            if(select(FD_SETSIZE, &sock_set, NULL, NULL, NULL) == -1)
                goto handleError;

            //If we're getting data from the pipe, it means that we're being terminated
            if(FD_ISSET(sock_pipe[0], &sock_set))
                return 0;
            else if(FD_ISSET(sock, &sock_set))
            {
#endif
                if((sent = recvfrom(sock, (char*)data, size, 0, (sockaddr*)&temp, &slen)) <= 0)
                    goto handleError;
#ifndef _WIN32
            }
            else
                goto handleError;
#endif

            //If the client doesn't exist, this is our client for the time being
            if(!you.sin_port) you = temp;

            //Is this the client? If so, break; if not, continue to read data
            if(you.sin_addr.s_addr == temp.sin_addr.s_addr) break;
        }

        return sent;

handleError:
#ifdef _WIN32
        int _error = neterrno;
        if(_error != WSAEINTR)
            neterror("Could not receive packet: %d", _error);
#else
        neterror("Could not receive packet: %d", neterrno);
#endif
        return 0;
#else
	return 0;
#endif
    }

    Player* getMyPlayer()
    {
#ifndef NO_NETWORK
        if(mode == MODE_SERVER) return &madotsuki;
        else if(mode == MODE_CLIENT) return &poniko;
#endif
        return NULL;
    }

    Player* getYourPlayer()
    {
#ifndef NO_NETWORK
        if(mode == MODE_SERVER) return &poniko;
        else if(mode == MODE_CLIENT) return &madotsuki;
#endif
        return NULL;
    }
}
