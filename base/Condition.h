#ifndef IAK_BASE_CONDITION_H
#define IAK_BASE_CONDITION_H

#include "NonCopyable.h"
#include "Mutex.h"

#include <stdint.h>
#include <pthread.h>

namespace iak {

class Condition : public NonCopyable {
public:
	explicit Condition(Mutex& mutex)
		: m_mutex(mutex) {
		::pthread_cond_init(&m_cond, NULL);
	}

	~Condition() {
		::pthread_cond_destroy(&m_cond);
	}

	void Signal() {
		::pthread_cond_signal(&m_cond);
	}

	void Broadcast() {
		::pthread_cond_broadcast(&m_cond);
	}

	void Wait() {
		::pthread_cond_wait(&m_cond, &m_mutex.GetPthreadMutex());
	}

	bool TimedWait(uint64_t timeout);

private:
	Mutex& m_mutex;
	pthread_cond_t m_cond;
};

} // end namespace iak

#endif // IAK_BASE_CONDITION_H
