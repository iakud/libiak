#ifndef IAK_LOG_LOGMESSAGE_H
#define IAK_LOG_LOGMESSAGE_H

#include "LogStream.h"

#include <chrono>

namespace iak {

enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NUM_LOG_LEVELS };

template<class Logger_>
class LogMessage {

public:
	LogMessage(Logger_& logger, const char* filename, int line, LogLevel level);
	LogMessage(Logger_& logger, const char* filename, int line, LogLevel level,
		const char* func);
	LogMessage(Logger_& logger, const char* filename, int line, bool toAbort);
	~LogMessage();

	LogStream& stream() { return stream_; }

private:
	std::chrono::system_clock::time_point time_;
	LogStream stream_;
	Logger_& logger_;
	const char* filename_;
	int line_;
	LogLevel level_;
}; // end class LogMessage


/*
template <typename T>
T* CheckNotNull(LogFilePtr logfile, const char* filename, int line, const char *name, T* ptr) {
	if (ptr == NULL) {
		LogLine(logfile, filename, line, LogLine::FATAL).stream() << name;
	}
	return ptr;
}
*/

/*
#define CHECK_NOTNULL(val) \
	::iak::CheckNotNull(__FILE__, __LINE__, "'" #val "' must be non NULL", (val))
*/

#define LOG_TRACE(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, ::iak::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, ::iak::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, ::iak::LogLevel::INFO).stream()
#define LOG_WARN(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, ::iak::LogLevel::WARN).stream()
#define LOG_ERROR(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, ::iak::LogLevel::ERROR).stream()
#define LOG_FATAL(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, ::iak::LogLevel::FATAL).stream()
#define LOG_SYSERR(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL(LOGGER) ::iak::LogMessage<decltype(LOGGER)>(LOGGER, __FILE__, __LINE__, true).stream()

} // end namespace iak

#endif  // IAK_LOG_LOGMESSAGE_H
