#ifndef IAK_BASE_MUTEX_H
#define IAK_BASE_MUTEX_H

#include "NonCopyable.h"
#include <pthread.h>

namespace iak {

class Mutex : public NonCopyable {
public:
	explicit Mutex() {
		::pthread_mutex_init(&m_mutex, NULL);
	}

	~Mutex() {
		::pthread_mutex_destroy(&m_mutex);
	}

	void Lock() {
		::pthread_mutex_lock(&m_mutex);
	}

	bool TryLock() {
		return !(::pthread_mutex_trylock(&m_mutex));// ret : EBUSY
	}

	void Unlock() {
		::pthread_mutex_unlock(&m_mutex);
	}

	pthread_mutex_t& GetPthreadMutex() {
		return m_mutex;
	}

private:
	pthread_mutex_t m_mutex;
};

class MutexGuard : public NonCopyable {
public:
	explicit MutexGuard(Mutex& mutex)
		: m_mutex(mutex) {
		m_mutex.Lock();
	}

	~MutexGuard() {
		m_mutex.Unlock();
	}

private:
	Mutex& m_mutex;
};

} // end namespace iak

#endif // IAK_BASE_MUTEX_H
