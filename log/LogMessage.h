#ifndef IAK_LOG_LOGMESSAGE_H
#define IAK_LOG_LOGMESSAGE_H

#include "LogStream.h"

#include <chrono>

namespace iak {
namespace log {

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
	::iak::log::CheckNotNull((LOGGER), __FILE__, __LINE__, "'" #val "' must be non NULL", (val))

#define DLOG_TRACE(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::log::LogLevel::TRACE, __func__).stream()
#define DLOG_DEBUG(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::log::LogLevel::DEBUG, __func__).stream()
#define DLOG_INFO(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::log::LogLevel::INFO).stream()
#define DLOG_WARN(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::log::LogLevel::WARN).stream()
#define DLOG_ERROR(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::log::LogLevel::ERROR).stream()
#define DLOG_FATAL(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, ::iak::log::LogLevel::FATAL).stream()
#define DLOG_SYSERR(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>((LOGGER), __FILE__, __LINE__, false).stream()
#define DLOG_SYSFATAL(LOGGER) ::iak::log::DLogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, true).stream()

#define LOG_INFO(LOGGER) ::iak::log::LogMessage<decltype(LOGGER)>((LOGGER), ::iak::log::LogLevel::INFO).stream()
#define LOG_ERROR(LOGGER) ::iak::log::LogMessage<decltype(LOGGER)>((LOGGER), ::iak::log::LogLevel::ERROR).stream()

} // end namespace log
} // end namespace iak

#endif  // IAK_LOG_LOGMESSAGE_H
