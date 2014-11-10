#include "LogFile.h"
#include "Logging.h" // strerror_tl
#include "AsyncLogger.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

namespace iak {

class AsyncLogFile::Buffer : public NonCopyable {

public:
	Buffer()
		: cur_(data_)
		, end_(data_ + sizeof data_) {
	}

	~Buffer() {
	}

	void append(const char* buf, size_t len) {
		if (static_cast<size_t>(avail()) > len) {
			::memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* data() const {
		return data_;
	}

	int length() const {
		return static_cast<int>(cur_ - data_);
	}

	int avail() const { return static_cast<int>(end_ - cur_); }

	void reset() { cur_ = data_; }

private:
	static const int kBufferSize = 4096 * 1024;

	char data_[kBufferSize];
	char* cur_;
	char* end_;
}; // end class AsyncLogFile::Buffer

} // end namespace iak

using namespace iak;

LogFilePtr AsyncLogFile::make(AsyncLogger* asyncLogger,
		const std::string& basename,
		size_t rollSize,
		int flushInterval) {
	return std::make_shared<AsyncLogFile>(asyncLogger, 
			basename, rollSize, flushInterval);
}

AsyncLogFile::AsyncLogFile(AsyncLogger* asyncLogger,
		const std::string& basename,
		size_t rollSize,
		int flushInterval)
	: LogFile(basename, rollSize, false, flushInterval)
	, asyncLogger_(asyncLogger)
	, currentBuffer_(std::make_shared<Buffer>())
	, nextBuffer_(std::make_shared<Buffer>())
	, buffers_() {
}

AsyncLogFile::~AsyncLogFile() {
}

void AsyncLogFile::append(const char* logline, int len) {
	asyncLogger->append(this, logline, len);
}

void AsyncLogFile::flush() {
	
}

void AsyncLogFile::append_unlocked(const char* logline, int len) {
	file_->append(logline, len);
	if (file_->writtenBytes() > rollSize_) {
		rollFile();
	} else {
		if (count_ > kCheckTimeRoll_) {
			count_ = 0;
			time_t now = ::time(NULL);
			time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
			if (thisPeriod_ != startOfPeriod_) {
				rollFile();
			}
		} else {
			++count_;
		}
	}
}

void LogFile::flush_unlocked() {
	file_->flush();
}

void LogFile::rollFile() {
	time_t now = 0;
	std::string filename = getLogFileName(basename_, &now);
	time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = start;
		file_.reset(new File(filename));
	}
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
	std::string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[32];
	char pidbuf[32];
	struct tm tm;
	*now = ::time(NULL);
	::localtime_r(now, &tm);
	::strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
	filename += timebuf;
	filename += ProcessInfo::hostName();
	::snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
	filename += pidbuf;
	filename += ".log";

	return filename;
}
