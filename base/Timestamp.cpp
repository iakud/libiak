#include "Timestamp.h"

#include <sys/time.h>
#include <stdio.h>

using namespace iak;

Timestamp Timestamp::Now() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}
