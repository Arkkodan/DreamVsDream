#include "thread.h"

static int thread_launcher(void (*func)()) {
	func();
	return 0;
}

//Thread
Thread::Thread() {
	thread = NULL;
}

Thread::Thread(void (*func)()) {
	//This prevents GCC from giving us warnings about breaking
	//ISO C standards.
	union
	{
		void* obj;
		void (*func)();
	} u;
	u.func = func;

	thread = SDL_CreateThread((int (*)(void*))thread_launcher, NULL, u.obj);
}

Thread::~Thread() {
}

void Thread::join() {
    SDL_WaitThread(thread, NULL);
}

//Mutex

Mutex::Mutex() {
    mutex = SDL_CreateMutex();
}

Mutex::~Mutex() {
    SDL_DestroyMutex(mutex);
}

void Mutex::lock() {
    SDL_LockMutex(mutex);
}

void Mutex::unlock() {
    SDL_UnlockMutex(mutex);
}
