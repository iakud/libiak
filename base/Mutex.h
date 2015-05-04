#ifndef IAK_BASE_MUTEX_H
#define IAK_BASE_MUTEX_H

#include <pthread.h>

namespace iak {

class Mutex {
public:
	explicit Mutex() {
		::pthread_mutex_init(&mutex_, NULL);
	}

	~Mutex() {
		::pthread_mutex_destroy(&mutex_);
	}
	// noncopyable
	Mutex(const Mutex&) = delete;
	Mutex& operator=(const Mutex&) = delete;

	void lock() {
		::pthread_mutex_lock(&mutex_);
	}

	bool trylock() {
		return !(::pthread_mutex_trylock(&mutex_));// ret : EBUSY
	}

	void unlock() {
		::pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t& getPthreadMutex() {
		return mutex_;
	}

private:
	pthread_mutex_t mutex_;
}; // end class Mutex

class MutexGuard {
public:
	explicit MutexGuard(Mutex& mutex)
		: mutex_(mutex) {
		mutex_.lock();
	}

	~MutexGuard() {
		mutex_.unlock();
	}
	// noncopyable
	MutexGuard(const MutexGuard&) = delete;
	MutexGuard& operator=(const MutexGuard&) = delete;

private:
	Mutex& mutex_;
}; // end class MutexGuard

} // end namespace iak

#endif // IAK_BASE_MUTEX_H
