#ifndef IAK_LOG_LOGGER_H
#define IAK_LOG_LOGGER_H

#include <mutex>
#include <string>
#include <memory>

namespace iak {

class Logger;
typedef std::shared_ptr<Logger> LoggerPtr;

class LogFile;

class Logger : public std::enable_shared_from_this<Logger> {

public:
	static LoggerPtr make(const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3,
			bool hostNameInLogFileName = false,
			bool pidInLogFileName = false);

protected:
	static const int kCheckTimeRoll_ = 1024;
	static const int kRollPerSeconds_ = 60 * 60 * 24;

public:
	Logger(const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3,
			bool hostNameInLogFileName = false,
			bool pidInLogFileName = false);
	~Logger();
	// noncopyable
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	void append(const char* logLine, int len);
	void flush();

private:
	void append_unlocked(const char* logLine, int len);
	void rollFile();

	const std::string basename_;
	const size_t rollSize_;
	const int flushInterval_;

	bool hostNameInLogFileName_;
	bool pidInLogFileName_;

	int count_;

	std::unique_ptr<std::mutex> mutexPtr_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;

	std::unique_ptr<LogFile> logFile_;
}; // end class Logger

} // end namespace iak

#endif	// IAK_LOG_LOGGER_H