#include "Condition.h"

using namespace iak;

const uint64_t NANOSEC_PER_SEC = 1e9;

bool Condition::timedwait(uint64_t timeout) {
	struct timespec abstime;
	::clock_gettime(CLOCK_REALTIME, &abstime);
	timeout += (abstime.tv_sec * NANOSEC_PER_SEC + abstime.tv_nsec);
	abstime.tv_sec = timeout / NANOSEC_PER_SEC;
	abstime.tv_nsec = timeout % NANOSEC_PER_SEC;
	return !(::pthread_cond_timedwait(&cond_, &mutex_.getPthreadMutex(), &abstime));
}
