#include "Logger.h"

#include "Thread.h"
#include "Timestamp.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

namespace iak {

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno) {
	return ::strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Logger::LogLevel GetLogLevel() {
	if (::getenv("MUDUO_LOG_TRACE"))
		return Logger::TRACE;
	else if (::getenv("MUDUO_LOG_DEBUG"))
		return Logger::DEBUG;
	else
		return Logger::INFO;
}

Logger::LogLevel Logger::s_level = GetLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
  "TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL "
};

void defaultOutput(const char* msg, int len) {
	size_t n = fwrite(msg, 1, len, stdout);
	//FIXME check n
	(void)n;
}

void defaultFlush() {
	fflush(stdout);
}

Logger::OutputFunc s_output = defaultOutput;
Logger::FlushFunc s_flush = defaultFlush;

} // end namespace iak

using namespace iak;

Logger::Logger(const char* filename, int line, LogLevel level)
	: time_(Timestamp::now())
	, stream_()
	, filename_(filename)
	, level_(level)
	, line_(line)
{
	int64_t time = time_.GetTime();
	time_t seconds = static_cast<time_t>(time / 1000000);
	int microseconds = static_cast<int>(time % 1000000);
	if (seconds != t_lastSecond) {
		t_lastSecond = seconds;
		struct tm tm_time;
		::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 17); (void)len;
	}
	LogFormat us(".%06dZ ", microseconds);
	assert(us.length() == 9);
	stream_ << t_time << us.data();
	stream_ << Thread::GetTidString();
	stream_ << LogLevelName[level];
}

Logger::Logger(const char* filename, int line, LogLevel level, const char* func)
	: Logger(filename, line, level) {
	stream_ << func << ' ';
}

Logger::Logger(SourceFile file, int line, bool toAbort)
	: Logger(filename, line, toAbort?FATAL:ERROR) {
	stream_ << Thread::GetTidString();
	stream_ << LogLevelName[level];
	int savedErrno = errno;
	if (savedErrno != 0) {
		stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

Logger::~Logger() {
	const char* slash = strrchr(filename_, '/');
	if (slash) {
		filename_ = slash + 1;
	}
	stream_ << " - " << filename_ << ':' << line_ << '\n';

	const LogStream::Buffer& buf(stream().buffer());
	g_output(buf.data(), buf.length());
	if (level_ == FATAL) {
		g_flush();
		abort();
	}
}
