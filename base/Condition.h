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

	void Signal() {
		::pthread_cond_signal(&cond_);
	}

	void Broadcast() {
		::pthread_cond_broadcast(&cond_);
	}

	void Wait() {
		::pthread_cond_wait(&cond_, &mutex_.GetPthreadMutex());
	}

	bool TimedWait(uint64_t timeout);

private:
	Mutex& mutex_;
	pthread_cond_t cond_;
};

} // end namespace iak

#endif // IAK_BASE_CONDITION_H
