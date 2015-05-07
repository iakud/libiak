#ifndef IAK_LOG_LOGGING_H
#define IAK_LOG_LOGGING_H

#include "LogStream.h"

#include <chrono>

namespace iak {

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

class ThisLogLevel {

public:
	static LogLevel GetLevel() { return s_level_; }
	static void SetLevel(LogLevel level) { s_level_ = level; }

private:
	static LogLevel s_level_;
};

template<class Logger_>
class Logging {

public:
	Logging(Logger_ logger, const char* filename, int line, LogLevel level);
	Logging(Logger_ logger, const char* filename, int line, LogLevel level,
			const char* func);
	Logging(Logger_ logger, const char* filename, int line, bool toAbort);
	~Logging();

	LogStream& stream() { return stream_; }

private:
	std::chrono::system_clock::time_point time_;
	LogStream stream_;
	Logger_ logger_;
	const char* filename_;
	int line_;
	LogLevel level_;
}; // end class Logging

/*
template <typename T>
T* CheckNotNull(LogFilePtr logfile, const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		LogLine(logfile, filename, line, LogLine::FATAL).stream() << name;
	}
	return ptr;
}
*/

#define LOG_TRACE(logger) if (::iak::ThisLogLevel::GetLevel() <= ::iak::LogLevel::TRACE) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, ::iak::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG(logger) if (::iak::ThisLogLevel::GetLevel() <= ::iak::LogLevel::DEBUG) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, ::iak::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO(logger) if (::iak::ThisLogLevel::GetLevel() <= ::iak::LogLevel::INFO) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, ::iak::LogLevel::INFO).stream()
#define LOG_WARN(logger) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, ::iak::LogLevel::WARN).stream()
#define LOG_ERROR(logger) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, ::iak::LogLevel::ERROR).stream()
#define LOG_FATAL(logger) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, ::iak::LogLevel::FATAL).stream()
#define LOG_SYSERR(logger) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL(logger) \
	::iak::Logging<decltype(logger)>(logger, __FILE__, __LINE__, true).stream()

/*
#define CHECK_NOTNULL(val) \
	::iak::CheckNotNull(__FILE__, __LINE__, "'" #val "' must be non NULL", (val))
*/

const char* strerror_tl(int savedErrno);

} // end namespace iak

#endif  // IAK_LOG_LOGGING_H
