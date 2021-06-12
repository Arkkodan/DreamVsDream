#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include <SDL_thread.h>

/// @brief Simple thread class
/// @details Uses SDL_Thread
class Thread {
public:
  Thread();
  Thread(Thread &&other) noexcept;
  Thread &operator=(Thread &&other) noexcept;
  ~Thread();

  // Don't copy threads
  Thread(const Thread &other) = delete;
  Thread &operator=(Thread &other) = delete;

  Thread(void (*func)());

  void detach();
  void join() const;

private:
  SDL_Thread *thread;
};

/// @brief Simple mutex class
/// @details Uses SDL_mutex
class Mutex {
public:
  Mutex();
  ~Mutex();

  // Don't copy mutexes period
  Mutex(Mutex &&other) = delete;
  Mutex &operator=(Mutex &&other) = delete;
  Mutex(const Mutex &other) = delete;
  Mutex &operator=(Mutex &other) = delete;

  void lock() const;
  void unlock() const;

private:
  SDL_mutex *mutex;
};

#endif // THREAD_H_INCLUDED
