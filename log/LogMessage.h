#ifndef IAK_LOG_LOGMESSAGE_H
#define IAK_LOG_LOGMESSAGE_H

#include "LogStream.h"

#include <chrono>

namespace iak {

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

template<class Logger_>
class DLogMessage {

public:
	DLogMessage(Logger_& logger, const char* filename, int line, LogLevel level);
	DLogMessage(Logger_& logger, const char* filename, int line, LogLevel level,
		const char* func);
	DLogMessage(Logger_& logger, const char* filename, int line, bool toAbort);
	~DLogMessage();

	LogStream& stream() { return stream_; }

private:
	LogStream stream_;
	Logger_& logger_;
	const char* filename_;
	int line_;
	LogLevel level_;
}; // end class DLogMessage

template<class Logger_>
class LogMessage {

public:
	LogMessage(Logger_& logger, LogLevel level);
	~LogMessage();

	LogStream& stream() { return stream_; }

private:
	LogStream stream_;
	Logger_& logger_;
}; // end class LogMessage

template <class Logger_, typename T>
T* CheckNotNull(Logger_& logger, const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		DLogMessage<decltype(logger)>(logger, filename, line, LogLevel::FATAL).stream() << name;
	}
	return ptr;
}

#define CHECK_NOTNULL(LOGGER,val) \
	::iak::CheckNotNull((LOGGER), __FILE__, __LINE__, "'" #val "' must be non NULL", (val))

#define DLOG_TRACE(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::LogLevel::TRACE, __func__).stream()
#define DLOG_DEBUG(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::LogLevel::DEBUG, __func__).stream()
#define DLOG_INFO(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::LogLevel::INFO).stream()
#define DLOG_WARN(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::LogLevel::WARN).stream()
#define DLOG_ERROR(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::LogLevel::ERROR).stream()
#define DLOG_FATAL(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::LogLevel::FATAL).stream()
#define DLOG_SYSERR(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, false).stream()
#define DLOG_SYSFATAL(LOGGER) ::iak::DLogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, true).stream()

#define LOG_INFO(LOGGER) ::iak::LogMessage<decltype(LOGGER)>((LOGGER), ::iak::LogLevel::INFO).stream()
#define LOG_ERROR(LOGGER) ::iak::LogMessage<decltype(LOGGER)>((LOGGER), ::iak::LogLevel::ERROR).stream()

} // end namespace iak

#endif  // IAK_LOG_LOGMESSAGE_H
