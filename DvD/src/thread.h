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

	void join();

private:
#ifdef _WIN32
	HANDLE thread;
#else
	pthread_t thread;
#endif
};

class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:
#ifdef _WIN32
	HANDLE mutex;
#else
	pthread_mutex_t mutex;
#endif
};

#endif //THREAD_H_INCLUDED
