#include <base/ProcessInfo.h>
#include <chrono>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace iak::base;

std::string formatTime(std::chrono::system_clock::time_point tp) {
	std::chrono::seconds seconds =
		std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
	std::chrono::microseconds microseconds =
		std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch() - seconds);
	time_t time = std::chrono::system_clock::to_time_t(tp);
	struct tm tm_time;
	::localtime_r(&time, &tm_time);
	char buf[32] = { 0 };
	::snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06ld",
		tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
		tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, static_cast<int64_t>(microseconds.count()));
	return buf;
}

int main() {
	printf("pid = %d\n", ProcessInfo::pid());
	printf("uid = %d\n", ProcessInfo::uid());
	printf("euid = %d\n", ProcessInfo::euid());

	std::string time = formatTime(ProcessInfo::startTime());
	
	printf("start time = %s\n", time.c_str());
	printf("hostname = %s\n", ProcessInfo::hostName().c_str());
	printf("opened files = %d\n", ProcessInfo::openedFiles());
	printf("threads = %zd\n", ProcessInfo::threads().size());
	printf("num threads = %d\n", ProcessInfo::numThreads());
	printf("status = %s\n", ProcessInfo::procStatus().c_str());
}
