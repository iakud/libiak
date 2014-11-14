#ifndef IAK_LOG_LOGFILE_H
#define IAK_LOG_LOGFILE_H

#include "AsyncLogging.h"

#include <base/NonCopyable.h>
#include <base/Mutex.h>

#include <string>
#include <memory>

namespace iak {

class LogFile;
typedef std::shared_ptr<LogFile> LogFilePtr;

class LogFile : public NonCopyable {

public:
	static LogFilePtr make(const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3);

	static LogFilePtr make(AsyncLogging* asyncLogging
			const std::string& basename,
			size_t rollSize);

public:
	LogFile(AsyncLogging* asyncLogging,
			const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3);
	~LogFile();

	void append(const char* logline, int len);
	void flush();

protected:
	static const int kCheckTimeRoll_ = 1024;
	static const int kRollPerSeconds_ = 60*60*24;
	static std::string getLogFileName(const std::string& basename, time_t* now);

	void append_unlocked(const char* logline, int len);
	void append_async(const std::string& logline);
	void rollFile();

	AsyncLogging* asyncLogging_;
	const std::string basename_;
	const size_t rollSize_;
	const int flushInterval_;

	int count_;

	std::unique_ptr<Mutex> mutex_;
	time_t startOfPeriod_;
	time_t lastRoll_;
	time_t lastFlush_;

	class File;
	std::unique_ptr<File> file_;
}; // end class LogFile

} // end namespace iak

#endif  // IAK_LOG_LOGFILE_H
