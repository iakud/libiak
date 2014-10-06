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
		: mutex_(mutex) {
		::pthread_cond_init(&cond_, NULL);
	}

	~Condition() {
		::pthread_cond_destroy(&cond_);
	}

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
	static const uint32_t kNanoSecondsPerSecond = 1e9;
	
	Mutex& mutex_;
	pthread_cond_t cond_;
};

} // end namespace iak

#endif // IAK_BASE_CONDITION_H
