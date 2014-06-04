#ifndef IAK_BASE_LOGGING_H
#define IAK_BASE_LOGGING_H

#include "LogStream.h"
#include "Timestamp.h"

namespace iak {

class Logger {
public:
	enum LogLevel {TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };
	typedef void (*OutputFunc)(const char* msg, int len);
	typedef void (*FlushFunc)();

	static LogLevel GetLevel() { return s_level_; }
	static void SetLevel(LogLevel level) { s_level_ = level; }
	static void SetOutput(OutputFunc output) { s_output_ = output; }
	static void SetFlush(FlushFunc flush) { s_flush_ = flush; }

public:
	Logger(const char* filename, int line, LogLevel level);
	Logger(const char* filename, int line, LogLevel level, const char* func);
	Logger(const char* filename, int line, bool toAbort);
	~Logger();

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
}; // end class Logger

template <typename T>
T* CheckNotNull(const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		Logger(filename, line, Logger::FATAL).GetStream() << name;
	}
	return ptr;
}

#define LOG_TRACE if (::iak::Logger::GetLevel() <= ::iak::Logger::TRACE) \
	::iak::Logger(__FILE__, __LINE__, ::iak::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (::iak::Logger::GetLevel() <= ::iak::Logger::DEBUG) \
	::iak::Logger(__FILE__, __LINE__, ::iak::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (::iak::Logger::GetLevel() <= ::iak::Logger::INFO) \
	::iak::Logger(__FILE__, __LINE__, ::iak::Logger::INFO).stream()
#define LOG_WARN ::iak::Logger(__FILE__, __LINE__, ::iak::Logger::WARN).stream()
#define LOG_ERROR ::iak::Logger(__FILE__, __LINE__, ::iak::Logger::ERROR).stream()
#define LOG_FATAL ::iak::Logger(__FILE__, __LINE__, ::iak::Logger::FATAL).stream()
#define LOG_SYSERR ::iak::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL ::iak::Logger(__FILE__, __LINE__, true).stream()

#define CHECK_NOTNULL(val) \
	::iak::CheckNotNull(__FILE__, __LINE__, "'" #val "' must be non NULL", (val))

const char* strerror_tl(int savedErrno);

} // end namespace iak

#endif  // IAK_BASE_LOGGER_H
