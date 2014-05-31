#ifndef IAK_BASE_LOGFILE_H
#define IAK_BASE_LOGFILE_H

#include "NonCopyable.h"
#include "Mutex.h"

#include <string>
#include <memory>

namespace iak {

class LogFile : public NonCopyable {
public:
	LogFile(const std::string& basename,
		size_t rollSize,
		bool threadSafe = true,
		int flushInterval = 3);
	~LogFile();

	void Append(const char* logline, int len);
	void Flush();

private:
	void append_unlocked(const char* logline, int len);
	void rollFile();
	static std::string getLogFileName(const std::string& basename, time_t* now);

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

	const static int kCheckTimeRoll_ = 1024;
	const static int kRollPerSeconds_ = 60*60*24;
}; // end class LogFile

} // end namespace iak

#endif  // IAK_BASE_LOGFILE_H
