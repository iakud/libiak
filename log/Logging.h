#ifndef IAK_LOG_LOGGING_H
#define IAK_LOG_LOGGING_H

#include "LogStream.h"
#include "LogFile.h"

#include <base/Timestamp.h>

namespace iak {

class Logging {

public:
	enum LogLevel {TRACE, DEBUG, INFO, WARNING, ERROR, FATAL, NUM_LOG_LEVELS };

	static LogLevel GetLevel() { return s_level_; }
	static void SetLevel(LogLevel level) { s_level_ = level; }

public:
	Logging(LogFilePtr logfile, const char* filename, int line, LogLevel level);
	Logging(LogFilePtr logfile, const char* filename, int line, LogLevel level,
			const char* func);
	Logging(LogFilePtr logfile, const char* filename, int line, bool toAbort);
	~Logging();

	LogStream& stream() { return stream_; }

private:
	static LogLevel s_level_;

	Timestamp time_;
	LogStream stream_;
	LogFilePtr logfile_;
	const char* filename_;
	int line_;
	LogLevel level_;
}; // end class Logging

template <typename T>
T* CheckNotNull(LogFilePtr logfile, const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		Logging(logfile, filename, line, Logging::FATAL).stream() << name;
	}
	return ptr;
}

#define LOG_TRACE(logfile) if (::iak::Logging::GetLevel() <= ::iak::Logging::TRACE) \
	::iak::Logging(logfile, __FILE__, __LINE__, ::iak::Logging::TRACE, __func__).stream()
#define LOG_DEBUG(logfile) if (::iak::Logging::GetLevel() <= ::iak::Logging::DEBUG) \
	::iak::Logging(logfile, __FILE__, __LINE__, ::iak::Logging::DEBUG, __func__).stream()
#define LOG_INFO(logfile) if (::iak::Logging::GetLevel() <= ::iak::Logging::INFO) \
	::iak::Logging(logfile, __FILE__, __LINE__, ::iak::Logging::INFO).stream()
#define LOG_WARNING(logfile) \
	::iak::Logging(logfile, __FILE__, __LINE__, ::iak::Logging::WARNING).stream()
#define LOG_ERROR(logfile) \
	::iak::Logging(logfile, __FILE__, __LINE__, ::iak::Logging::ERROR).stream()
#define LOG_FATAL(logfile) \
	::iak::Logging(logfile, __FILE__, __LINE__, ::iak::Logging::FATAL).stream()
#define LOG_SYSERR(logfile) \
	::iak::Logging(logfile, __FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL(logfile) \
	::iak::Logging(logfile, __FILE__, __LINE__, true).stream()

#define CHECK_NOTNULL(val) \
	::iak::CheckNotNull(__FILE__, __LINE__, "'" #val "' must be non NULL", (val))

const char* strerror_tl(int savedErrno);

} // end namespace iak

#endif  // IAK_LOG_LOGGING_H
