#include "LogFile.h"
#include "Logging.h" // strerror_tl
#include "AsyncLogger.h"

#include <base/ProcessInfo.h>

#include <assert.h>
#include <stdio.h>
#include <time.h>

namespace iak {

// not thread safe
class LogFile::File : public NonCopyable {
public:
	explicit File(const std::string& filename)
		: fp_(::fopen(filename.data(), "ae"))
		, writtenBytes_(0) {
		assert(fp_);
		::setbuffer(fp_, buffer_, sizeof buffer_);
		// posix_fadvise POSIX_FADV_DONTNEED ?
	}

	~File() {
		::fclose(fp_);
	}

	void append(const char* logline, const size_t len) {
		size_t n = write(logline, len);
		size_t remain = len - n;
		while (remain > 0) {
			size_t x = write(logline + n, remain);
			if (x == 0) {
				int err = ferror(fp_);
				if (err) {
					fprintf(stderr, "LogFile::File::append() failed %s\n", strerror_tl(err));
				}
				break;
			}
			n += x;
			remain = len - n; // remain -= x
		}
		writtenBytes_ += len;
	}

	void flush() {
		::fflush(fp_);
	}

	size_t writtenBytes() const { return writtenBytes_; }

private:
	size_t write(const char* logline, size_t len) {
		return ::fwrite_unlocked(logline, 1, len, fp_);
	}

	FILE* fp_;
	char buffer_[64*1024];
	size_t writtenBytes_;
}; // end class File

} // end namespace iak

using namespace iak;

LogFilePtr LogFile::make(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize,
		int flushInterval) {
	return std::make_shared<LogFile>(basename,
			rollSize, async, flushInterval);
}

LogFile::LogFile(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize,
		int flushInterval)
	: asyncLogging_(asyncLogging)
	, basename_(basename)
	, rollSize_(rollSize)
	, flushInterval_(flushInterval)
	, count_(0)
	, mutex_(asyncLogging_ == nullptr ? new Mutex : nullptr)
	, startOfPeriod_(0)
	, lastRoll_(0)
	, lastFlush_(0) 
	, currentBuffer_(asyncLogging_ == nullptr ? nullptr : LogBuffer::make())
	, nextBuffer_(asyncLogging_ == nullptr ? nullptr : LogBuffer::make())
	, currentBufferBackup_(asyncLogging_ == nullptr ? nullptr : LogBuffer::make())
	, nextBufferBackup_(asyncLogging_ == nullptr ? nullptr : LogBuffer::make()) {
	assert(basename.find('/') == std::string::npos);
	rollFile();
}

LogFile::~LogFile() {
}

void LogFile::append(const char* logline, int len) {
	if (asyncLogging_ == nullptr) {
		MutexGuard lock(*mutex_);
		append_unlocked(logline, len);
	} else {
		AsyncLogger::append(shared_from_this(), logline, len);
	}
}

void LogFile::flush() {
	if (asyncLogging_ == nullptr) {
		MutexGuard lock(*mutex_);
		file_->flush();
	}
}

void LogFile::append_unlocked(const char* logline, int len) {
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
			} else if (now - lastFlush_ > flushInterval_) {
				lastFlush_ = now;
				file_->flush();
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
