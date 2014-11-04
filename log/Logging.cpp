#include "Logging.h"

#include <base/Thread.h>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace iak {

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno) {
	return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Logging::LogLevel GetLogLevel() {
	if (::getenv("IAK_LOG_TRACE"))
		return Logging::TRACE;
	else if (::getenv("IAK_LOG_DEBUG"))
		return Logging::DEBUG;
	else
		return Logging::INFO;
}

Logging::LogLevel Logging::s_level_ = GetLogLevel();

const char* LogLevelName[Logging::NUM_LOG_LEVELS] = {
  "TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL "
};

void defaultOutput(const char* msg, int len) {
	size_t n = ::fwrite(msg, 1, len, stdout);
	//FIXME check n
	(void)n;
}

void defaultFlush() {
	::fflush(::stdout);
}

Logging::OutputFunc Logging::s_output_ = defaultOutput;
Logging::FlushFunc Logging::s_flush_ = defaultFlush;

} // end namespace iak

using namespace iak;

Logging::Logging(const char* filename, int line, LogLevel level)
	: time_(Timestamp::now())
	, stream_()
	, filename_(filename)
	, line_(line)
	, level_(level) {
	int64_t time = time_.getTime();
	time_t seconds = static_cast<time_t>(time / Timestamp::kMicroSecondsPerSecond);
	int microseconds = static_cast<int>(time % Timestamp::kMicroSecondsPerSecond);
	if (seconds != t_lastSecond) {
		t_lastSecond = seconds;
		struct tm tm_time;
		//::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
		::localtime_r(&seconds, &tm_time);
		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 17); (void)len;
	}
	LogFormat us(".%06dZ ", microseconds);
	assert(us.length() == 9);
	stream_ << t_time <<us.data();
	stream_ << Thread::tidString();
	stream_ << LogLevelName[level];
}

Logging::Logging(const char* filename, int line, LogLevel level, const char* func)
	: Logging(filename, line, level) {
	stream_ << func << ' ';
}

Logging::Logging(const char* filename, int line, bool toAbort)
	: Logging(filename, line, toAbort?FATAL:ERROR) {
	int savedErrno = errno;
	if (savedErrno != 0) {
		stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

Logging::~Logging() {
	const char* slash = ::strrchr(filename_, '/');
	if (slash) {
		filename_ = slash + 1;
	}
	stream_ << " - " << filename_ << ':' << line_ << '\n';

	s_output_(stream_.data(), stream_.length());
	if (level_ == FATAL) {
		s_flush_();
		::abort();
	}
}
