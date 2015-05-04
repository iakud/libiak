#ifndef IAK_BASE_CONDITION_H
#define IAK_BASE_CONDITION_H

#include "Mutex.h"

#include <stdint.h>
#include <pthread.h>

namespace iak {

class Condition {

private:
	static const uint32_t kNanoSecondsPerSecond = 1e9;

public:
	explicit Condition(Mutex& mutex)
		: mutex_(mutex) {
		::pthread_cond_init(&cond_, NULL);
	}

	~Condition() {
		::pthread_cond_destroy(&cond_);
	}
	// noncopyable
	Condition(const Condition&) = delete;
	Condition& operator=(const Condition&) = delete;

	void signal() {
		::pthread_cond_signal(&cond_);
	}

	void broadcast() {
		::pthread_cond_broadcast(&cond_);
	}

	void wait() {
		::pthread_cond_wait(&cond_, &mutex_.getPthreadMutex());
	}

	/* timedwait using
	 * a nanoseconds time value.*/
	bool timedwait(uint64_t timeout);

private:
	Mutex& mutex_;
	pthread_cond_t cond_;
}; // end class Condition

} // end namespace iak

#endif // IAK_BASE_CONDITION_H
