#ifndef IAK_LOG_ASYNCLOGGER_H
#define IAK_LOG_ASYNCLOGGER_H

#include <string>
#include <memory>

namespace iak {

class AsyncLogger;
typedef std::shared_ptr<AsyncLogger> AsyncLoggerPtr;

class LogFile;
class AsyncLogging;

class AsyncLogger : public std::enable_shared_from_this<AsyncLogger> {

public:
	static AsyncLoggerPtr make(AsyncLogging* asyncLogging,
			const std::string& basename,
			size_t rollSize,
			int flushInterval = 3);

protected:
	static const int kCheckTimeRoll_ = 1024;
	static const int kRollPerSeconds_ = 60 * 60 * 24;

public:
	AsyncLogger(AsyncLogging* asyncLogging,
			const std::string& basename,
			size_t rollSize,
			int flushInterval = 3);
	~AsyncLogger();
	// noncopyable
	AsyncLogger(const AsyncLogger&) = delete;
	AsyncLogger& operator=(const AsyncLogger&) = delete;

	void append(const char* logline, int len);
	void flush();

protected:
	void append_unlocked(const char* logline, int len);
	void rollFile();

	AsyncLogging* asyncLogging_;
	const std::string basename_;
	const size_t rollSize_;
	const int flushInterval_;

	int count_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;

	std::unique_ptr<LogFile> logFile_;
}; // end class AsyncLogger

} // end namespace iak

#endif  // IAK_LOG_ASYNCLOGGER_H
