#include "LogMessage.h"
#include "Logger.h"
#include "Logging.h"
//#include "AsyncLogger.h"

#include <thread>

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace iak {

thread_local char t_time[32];
thread_local time_t t_lastSecond;

const char* LogLevelName[static_cast<int>(LogLevel::NUM_LOG_LEVELS)] = {
	"TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL "
};

} // end namespace iak

using namespace iak;

template<class Logger_>
LogMessage<Logger_>::LogMessage(Logger_ logger, const char* filename, int line, LogLevel level)
	: time_(std::chrono::system_clock::now())
	, stream_()
	, logger_(logger)
	, filename_(filename)
	, line_(line)
	, level_(level) {
	
	std::chrono::seconds seconds = 
		std::chrono::duration_cast<std::chrono::seconds>(time_.time_since_epoch());
	std::chrono::microseconds microseconds = 
		std::chrono::duration_cast<std::chrono::microseconds>(time_.time_since_epoch() - seconds);

	time_t time = std::chrono::system_clock::to_time_t(time_);
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
LogMessage<Logger_>::LogMessage(Logger_ logger, const char* filename, int line, LogLevel level, const char* func)
	: LogMessage(logger, filename, line, level) {
	stream_ << func << ' ';
}

template<class Logger_>
LogMessage<Logger_>::LogMessage(Logger_ logger, const char* filename, int line, bool toAbort)
	: LogMessage(logger, filename, line, toAbort ? LogLevel::FATAL : LogLevel::ERROR) {
	int savedErrno = errno;
	if (savedErrno != 0) {
		stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
	}
}

template<class Logger_>
LogMessage<Logger_>::~LogMessage() {
	if (logger_) {
		if (logger_->isFileAndLineInLog()) {
			const char* slash = ::strrchr(filename_, '/');
			if (slash) {
				filename_ = slash + 1;
			}
			stream_ << " - " << filename_ << ':' << line_;
		}
		stream_ << '\n';
		logger_->append(stream_.data(), stream_.length());
	} else {
		const char* slash = ::strrchr(filename_, '/');
		if (slash) {
			filename_ = slash + 1;
		}
		stream_ << " - " << filename_ << ':' << line_ << '\n';
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

//  explicit instantiations
template class LogMessage<LoggerPtr>;
//template class LogMessage<AsyncLogger*>;