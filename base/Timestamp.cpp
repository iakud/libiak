#include "Timestamp.h"

#include <sys/time.h>
#include <stdio.h>

using namespace iak;

#define MICROSEC_PER_SEC ((uint32_t)1e6)

Timestamp Timestamp::Now() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return Timestamp(tv.tv_sec * MICROSEC_PER_SEC + tv.tv_usec);
}
