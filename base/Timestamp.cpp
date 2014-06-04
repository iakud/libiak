#include "Timestamp.h"

#include <sys/time.h>
#include <stdio.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

namespace iak {

const uint32_t MICROSEC_PER_SEC = 1e6;

} // end namespace iak

using namespace iak;

Timestamp Timestamp::now() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return Timestamp(tv.tv_sec * MICROSEC_PER_SEC + tv.tv_usec);
}

std::string Timestamp::toString() const {
	char buf[32] = {0};
	int64_t seconds = time_ / MICROSEC_PER_SEC;
	int64_t microseconds = time_ % MICROSEC_PER_SEC;
	::snprintf(buf, sizeof buf - 1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

std::string Timestamp::toFormattedString() const {
	char buf[32] = {0};
	time_t seconds = static_cast<time_t>(time_ / MICROSEC_PER_SEC);
	int microseconds = static_cast<int>(time_ % MICROSEC_PER_SEC);
	struct tm tm_time;
	::localtime_r(&seconds, &tm_time);

	::snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
	return buf;
}
