#include "thread.h"

#ifdef _WIN32
static DWORD thread_launcher(void (*func)()) {
	func();
	return 0;
}
#else
static void* thread_launcher(void (*func)()) {
	func();
	pthread_exit(NULL);
}
#endif

//Thread
Thread::Thread() {
	memset(&thread, 0, sizeof(thread));
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

#ifdef _WIN32
	thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_launcher, u.obj, 0, NULL);
#else
	pthread_create(&thread, NULL, (void* (*)(void*))thread_launcher, u.obj);
#endif
}

Thread::~Thread() {
#ifdef _WIN32
	CloseHandle(thread);
#endif
}

void Thread::join()
{
#ifdef _WIN32
	WaitForSingleObject(thread, INFINITE);
#else
	pthread_join(thread, NULL);
#endif
}

//Mutex

Mutex::Mutex() {
#ifdef _WIN32
	mutex = CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutex_init(&mutex, NULL);
#endif
}

Mutex::~Mutex() {
#ifdef _WIN32
	CloseHandle(mutex);
#else
	pthread_mutex_destroy(&mutex);
#endif
}

void Mutex::lock() {
#ifdef _WIN32
	WaitForSingleObject(mutex, INFINITE);
#else
	pthread_mutex_lock(&mutex);
#endif
}

void Mutex::unlock() {
#ifdef _WIN32
	ReleaseMutex(mutex);
#else
	pthread_mutex_unlock(&mutex);
#endif
}
