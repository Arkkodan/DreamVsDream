#include "thread.h"

static int thread_launcher(void (*func)()) {
  func();
  return 0;
}

// Thread
Thread::Thread() { thread = nullptr; }

Thread::Thread(Thread &&other) noexcept { thread = other.thread; }

Thread &Thread::operator=(Thread &&other) noexcept {
  thread = other.thread;
  return *this;
}

Thread::Thread(void (*func)()) {
  thread =
      SDL_CreateThread((int (*)(void *))thread_launcher, nullptr, (void *)func);
}

Thread::~Thread() {}

void Thread::join() const {
  if (thread) {
    SDL_WaitThread(thread, nullptr);
  }
}

// Mutex

Mutex::Mutex() : mutex(nullptr) {
  // mutex = SDL_CreateMutex();
}

Mutex::~Mutex() {
  // SDL_DestroyMutex(mutex);
}

void Mutex::lock() const {
  // SDL_LockMutex(mutex);
}

void Mutex::unlock() const {
  // SDL_UnlockMutex(mutex);
}
