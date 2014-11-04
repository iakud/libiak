#ifndef IAK_BASE_LOGFILE_H
#define IAK_BASE_LOGFILE_H

#include <base/NonCopyable.h>
#include <base/Mutex.h>

#include <string>
#include <memory>

namespace iak {

class LogFile;
typedef std::shared_ptr<LogFile> LogFilePtr;

class LogFile : public NonCopyable,
	public std::enable_shared_from_this<LogFile> {

public:
	static LogFilePtr make(const std::string& basename,
			size_t rollSize,
			bool async = true,
			int flushInterval = 3);

public:
	LogFile(const std::string& basename,
			size_t rollSize,
			bool async,
			int flushInterval);
	~LogFile();

	void append(const char* logline, int len);
	void flush();

private:
	static const int kCheckTimeRoll_ = 1024;
	static const int kRollPerSeconds_ = 60*60*24;
	static std::string getLogFileName(const std::string& basename, time_t* now);

	void append_unlocked(const char* logline, int len);
	void flush_unlocked();
	void rollFile();

	const std::string basename_;
	const size_t rollSize_;
	const bool async_;
	const int flushInterval_;

	int count_;

	std::unique_ptr<Mutex> mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;

	class File;
	std::unique_ptr<File> file_;

	friend class AsyncLogger;
}; // end class LogFile

} // end namespace iak

#endif  // IAK_BASE_LOGFILE_H
