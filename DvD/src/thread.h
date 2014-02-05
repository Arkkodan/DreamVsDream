#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#ifndef _WIN32
#include <pthread.h>
#endif

#include "globals.h"

class Thread {
public:
	Thread();
	Thread(void (*func)());
	~Thread();

    void detach();
	void join();

private:
    SDL_Thread* thread;
};

class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
    SDL_mutex* mutex;
};

#endif //THREAD_H_INCLUDED
