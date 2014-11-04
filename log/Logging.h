#ifndef IAK_BASE_LOGGING_H
#define IAK_BASE_LOGGING_H

#include "LogStream.h"

#include <base/Timestamp.h>

namespace iak {

class Logging {
public:
	enum LogLevel {TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };
	typedef void (*OutputFunc)(const char* msg, int len);
	typedef void (*FlushFunc)();

	static LogLevel GetLevel() { return s_level_; }
	static void SetLevel(LogLevel level) { s_level_ = level; }
	static void SetOutput(OutputFunc output) { s_output_ = output; }
	static void SetFlush(FlushFunc flush) { s_flush_ = flush; }

public:
	Logging(const char* filename, int line, LogLevel level);
	Logging(const char* filename, int line, LogLevel level, const char* func);
	Logging(const char* filename, int line, bool toAbort);
	~Logging();

	LogStream& stream() { return stream_; }

private:
	static LogLevel s_level_;
	static OutputFunc s_output_;
	static FlushFunc s_flush_;

	Timestamp time_;
	LogStream stream_;
	const char* filename_;
	int line_;
	LogLevel level_;
}; // end class Logging

template <typename T>
T* CheckNotNull(const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		Logging(filename, line, Logging::FATAL).stream() << name;
	}
	return ptr;
}

#define LOG_TRACE if (::iak::Logging::GetLevel() <= ::iak::Logging::TRACE) \
	::iak::Logging(__FILE__, __LINE__, ::iak::Logging::TRACE, __func__).stream()
#define LOG_DEBUG if (::iak::Logging::GetLevel() <= ::iak::Logging::DEBUG) \
	::iak::Logging(__FILE__, __LINE__, ::iak::Logging::DEBUG, __func__).stream()
#define LOG_INFO if (::iak::Logging::GetLevel() <= ::iak::Logging::INFO) \
	::iak::Logging(__FILE__, __LINE__, ::iak::Logging::INFO).stream()
#define LOG_WARN ::iak::Logging(__FILE__, __LINE__, ::iak::Logging::WARN).stream()
#define LOG_ERROR ::iak::Logging(__FILE__, __LINE__, ::iak::Logging::ERROR).stream()
#define LOG_FATAL ::iak::Logging(__FILE__, __LINE__, ::iak::Logging::FATAL).stream()
#define LOG_SYSERR ::iak::Logging(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL ::iak::Logging(__FILE__, __LINE__, true).stream()

#define CHECK_NOTNULL(val) \
	::iak::CheckNotNull(__FILE__, __LINE__, "'" #val "' must be non NULL", (val))

const char* strerror_tl(int savedErrno);

} // end namespace iak

#endif  // IAK_BASE_LOGGING_H
