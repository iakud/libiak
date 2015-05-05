#include <base/ProcessInfo.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace iak;

int main() {
	printf("pid = %d\n", ProcessInfo::pid());
	printf("uid = %d\n", ProcessInfo::uid());
	printf("euid = %d\n", ProcessInfo::euid());

	std::chrono::system_clock::time_point startTime = ProcessInfo::startTime();
	std::chrono::seconds seconds = 
		std::chrono::duration_cast<std::chrono::seconds>(startTime.time_since_epoch());
	std::chrono::microseconds microseconds = 
		std::chrono::duration_cast<std::chrono::microseconds>(startTime.time_since_epoch() - seconds);
	time_t time = std::chrono::system_clock::to_time_t(startTime);
	struct tm tm_time;
	::localtime_r(&time, &tm_time);
	char buf[32] = { 0 };
	::snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06ld",
		tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds.count());
	printf("start time = %s\n", buf);
	printf("hostname = %s\n", ProcessInfo::hostName().c_str());
	printf("opened files = %d\n", ProcessInfo::openedFiles());
	printf("threads = %zd\n", ProcessInfo::threads().size());
	printf("num threads = %d\n", ProcessInfo::numThreads());
	printf("status = %s\n", ProcessInfo::procStatus().c_str());
}
