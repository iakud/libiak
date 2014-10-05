#include "Condition.h"

using namespace iak;

bool Condition::timedwait(unsigned int timeout) {
	struct timespec abstime;
	::clock_gettime(CLOCK_REALTIME, &abstime);
	timeout = timeout * 1000 + abstime.tv_sec * kNanoSecondsPerSecond + abstime.tv_nsec;
	abstime.tv_sec = timeout / kNanoSecondsPerSecond;
	abstime.tv_nsec = timeout % kNanoSecondsPerSecond;
	return !(::pthread_cond_timedwait(&cond_, &mutex_.getPthreadMutex(), &abstime));
}
