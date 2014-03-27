#include "thread.h"

static int thread_launcher(void (*func)()) {
	func();
	return 0;
}

//Thread
Thread::Thread() {
	thread = nullptr;
}

Thread::Thread(Thread&& other) {
    thread = other.thread;
}

Thread& Thread::operator=(Thread&& other) {
    thread = other.thread;
    return *this;
}

Thread::Thread(void (*func)()) {
	thread = SDL_CreateThread((int (*)(void*))thread_launcher, nullptr, (void*)func);
}

Thread::~Thread() {
}

void Thread::join() {
    if(thread) {
        SDL_WaitThread(thread, nullptr);
    }
}

//Mutex

Mutex::Mutex() {
    //mutex = SDL_CreateMutex();
}

Mutex::~Mutex() {
    //SDL_DestroyMutex(mutex);
}

void Mutex::lock() {
    //SDL_LockMutex(mutex);
}

void Mutex::unlock() {
    //SDL_UnlockMutex(mutex);
}
