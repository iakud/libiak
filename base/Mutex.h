#ifndef IAK_BASE_MUTEX_H
#define IAK_BASE_MUTEX_H

#include "NonCopyable.h"

#include <pthread.h>

namespace iak {

class Mutex : public NonCopyable {
public:
	explicit Mutex() {
		::pthread_mutex_init(&mutex_, NULL);
	}

	~Mutex() {
		::pthread_mutex_destroy(&mutex_);
	}

	void Lock() {
		::pthread_mutex_lock(&mutex_);
	}

	bool TryLock() {
		return !(::pthread_mutex_trylock(&mutex_));// ret : EBUSY
	}

	void Unlock() {
		::pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t& GetPthreadMutex() {
		return mutex_;
	}

private:
	pthread_mutex_t mutex_;
};

class MutexGuard : public NonCopyable {
public:
	explicit MutexGuard(Mutex& mutex)
		: mutex_(mutex) {
		mutex_.Lock();
	}

	~MutexGuard() {
		mutex_.Unlock();
	}

private:
	Mutex& mutex_;
};

} // end namespace iak

#endif // IAK_BASE_MUTEX_H
