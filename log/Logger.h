#ifndef IAK_LOG_LOGGER_H
#define IAK_LOG_LOGGER_H

#include "LogStream.h"
#include "LogFile.h"

#include <chrono>

namespace iak {

class Logger {

public:
	enum LogLevel {TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

	static LogLevel GetLevel() { return s_level_; }
	static void SetLevel(LogLevel level) { s_level_ = level; }

private:
	static LogLevel s_level_;

public:
	Logger(LogFilePtr logfile, const char* filename, int line, LogLevel level);
	Logger(LogFilePtr logfile, const char* filename, int line, LogLevel level,
			const char* func);
	Logger(LogFilePtr logfile, const char* filename, int line, bool toAbort);
	~Logger();

	LogStream& stream() { return stream_; }

private:
	std::chrono::system_clock::time_point time_;
	LogStream stream_;
	LogFilePtr logfile_;
	const char* filename_;
	int line_;
	LogLevel level_;
}; // end class Logger

template <typename T>
T* CheckNotNull(LogFilePtr logfile, const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		Logger(logfile, filename, line, Logger::FATAL).stream() << name;
	}
	return ptr;
}

#define LOG_TRACE(logfile) if (::iak::Logger::GetLevel() <= ::iak::Logger::TRACE) \
	::iak::Logger(logfile, __FILE__, __LINE__, ::iak::Logger::TRACE, __func__).stream()
#define LOG_DEBUG(logfile) if (::iak::Logger::GetLevel() <= ::iak::Logger::DEBUG) \
	::iak::Logger(logfile, __FILE__, __LINE__, ::iak::Logger::DEBUG, __func__).stream()
#define LOG_INFO(logfile) if (::iak::Logger::GetLevel() <= ::iak::Logger::INFO) \
	::iak::Logger(logfile, __FILE__, __LINE__, ::iak::Logger::INFO).stream()
#define LOG_WARN(logfile) \
	::iak::Logger(logfile, __FILE__, __LINE__, ::iak::Logger::WARN).stream()
#define LOG_ERROR(logfile) \
	::iak::Logger(logfile, __FILE__, __LINE__, ::iak::Logger::ERROR).stream()
#define LOG_FATAL(logfile) \
	::iak::Logger(logfile, __FILE__, __LINE__, ::iak::Logger::FATAL).stream()
#define LOG_SYSERR(logfile) \
	::iak::Logger(logfile, __FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL(logfile) \
	::iak::Logger(logfile, __FILE__, __LINE__, true).stream()

#define CHECK_NOTNULL(val) \
	::iak::CheckNotNull(__FILE__, __LINE__, "'" #val "' must be non NULL", (val))

const char* strerror_tl(int savedErrno);

} // end namespace iak

#endif  // IAK_LOG_LOGGER_H
