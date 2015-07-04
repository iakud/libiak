#include "LogMessage.h"
#include "Logger.h"
#include "Logging.h"
#include "AsyncLogger.h"

#include <thread>

#include <assert.h>
#include <errno.h>

namespace iak {
namespace log {

thread_local char t_time[32];
thread_local int64_t t_lastSecond;

const char* LogLevelName[static_cast<int>(LogLevel::NUM_LOG_LEVELS)] = {
	"TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL "
};

} // end namespace log
} // end namespace iak

using namespace iak::log;

// DLogMessage
template<class Logger_>
DLogMessage<Logger_>::DLogMessage(Logger_& logger, const char* filename, int line, LogLevel level)
	: stream_()
	, logger_(logger)
	, filename_(filename)
	, line_(line)
	, level_(level) {
	
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::seconds seconds = 
		std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
	std::chrono::microseconds microseconds = 
		std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch() - seconds);

	time_t time = std::chrono::system_clock::to_time_t(now);
	if (seconds.count() != t_lastSecond) {
		t_lastSecond = seconds.count();
		struct tm tm_time;
		//::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
		::localtime_r(&time, &tm_time);
		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 17); (void)len;
	}
	LogFormat us(".%06d ", microseconds.count());
	assert(us.length() == 8);
	stream_ << t_time <<us.data();
	// stream_ << Thread::tidString(); // FIXME:std::this_thread::get_id()
	stream_ << LogLevelName[static_cast<int>(level)];
}

template<class Logger_>
DLogMessage<Logger_>::DLogMessage(Logger_& logger, const char* filename, int line, LogLevel level, const char* func)
	: DLogMessage(logger, filename, line, level) {
	stream_ << func << ' ';
}

template<class Logger_>
DLogMessage<Logger_>::DLogMessage(Logger_& logger, const char* filename, int line, bool toAbort)
	: DLogMessage(logger, filename, line, toAbort ? LogLevel::FATAL : LogLevel::ERROR) {
	int savedErrno = errno;
	if (savedErrno != 0) {
		stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

template<class Logger_>
DLogMessage<Logger_>::~DLogMessage() {
	const char* slash = ::strrchr(filename_, '/');
	if (slash) {
		filename_ = slash + 1;
	}
	stream_ << " - " << filename_ << ':' << line_ << '\n';
	if (logger_) {
		logger_->append(stream_.data(), stream_.length());
	} else {
		::fwrite(stream_.data(), 1, stream_.length(), stdout);
	}

	if (level_ == LogLevel::FATAL) {
		if (logger_) {
			logger_->flush();
		} else {
			::fflush(::stdout);
		}
		::abort();
	}
}

// LogMessage
template<class Logger_>
LogMessage<Logger_>::LogMessage(Logger_& logger, LogLevel level)
	: stream_()
	, logger_(logger) {

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::seconds seconds =
		std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
	std::chrono::microseconds microseconds =
		std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch() - seconds);

	time_t time = std::chrono::system_clock::to_time_t(now);
	if (seconds.count() != t_lastSecond) {
		t_lastSecond = seconds.count();
		struct tm tm_time;
		::localtime_r(&time, &tm_time);
		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
			tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
			tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
		assert(len == 17); (void)len;
	}
	LogFormat us(".%06d ", microseconds.count());
	assert(us.length() == 8);
	stream_ << t_time << us.data();
	// stream_ << Thread::tidString(); // FIXME:std::this_thread::get_id()
	stream_ << LogLevelName[static_cast<int>(level)];
}

template<class Logger_>
LogMessage<Logger_>::~LogMessage() {
	stream_ << '\n';
	if (logger_) {
		logger_->append(stream_.data(), stream_.length());
	} else {
		::fwrite(stream_.data(), 1, stream_.length(), stdout);
	}
}

//  explicit instantiations
template class DLogMessage<LoggerPtr>;
template class DLogMessage<AsyncLoggerPtr>;
template class LogMessage<LoggerPtr>;
template class LogMessage<AsyncLoggerPtr>;