#ifndef IAK_LOG_ASYNCLOGGER_H
#define IAK_LOG_ASYNCLOGGER_H

#include <string>
#include <memory>

namespace iak {

class AsyncLogger;
typedef std::shared_ptr<AsyncLogger> AsyncLoggerPtr;

class LogFile;

class AsyncLogger : public std::enable_shared_from_this<AsyncLogger> {

public:
	static AsyncLoggerPtr make(AsyncLogging* asyncLogging,
			const std::string& basename,
			size_t rollSize,
			int flushInterval = 3)

	static void setLogDir(const std::string& dir);
	static void setHostInLogFileName(bool host);
	static void setPidInLogFileName(bool pid);
protected:
	static const int kCheckTimeRoll_ = 1024;
	static const int kRollPerSeconds_ = 60 * 60 * 24;
	static std::string getLogFileName(const std::string& basename, time_t* now);
	static std::string s_dir_;
	static bool s_host_;
	static bool s_pid_;

public:
	LogFile(AsyncLogging* asyncLogging,
			const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3);
	~LogFile();
	// noncopyable
	LogFile(const LogFile&) = delete;
	LogFile& operator=(const LogFile&) = delete;

	void append(const char* logline, int len);
	void flush();

protected:
	void append_unlocked(const char* logline, int len);
	void append_async(const std::string& logline);
	void rollFile();

	AsyncLogging* asyncLogging_;
	const std::string basename_;
	const size_t rollSize_;
	const int flushInterval_;

	int count_;

	std::unique_ptr<std::mutex> mutexPtr_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;

	std::unique_ptr<LogFile> file_;
}; // end class LogFile

} // end namespace iak

#endif  // IAK_LOG_ASYNCLOGGER_H
