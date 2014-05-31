#include "LogFile.h"
#include "Logger.h" // strerror_tl
//#include <muduo/base/ProcessInfo.h>

#include <assert.h>
#include <stdio.h>
#include <time.h>

namespace iak {
// not thread safe
class LogFile::File : ppublic NonCopyable {
public:
	explicit File(const string& filename)
		: fp_(::fopen(filename.data(), "ae"))
		, writtenBytes_(0) {
		assert(fp_);
		::setbuffer(fp_, buffer_, sizeof buffer_);
		// posix_fadvise POSIX_FADV_DONTNEED ?
	}

	~File() {
		::fclose(fp_);
	}

	void Append(const char* logline, const size_t len) {
		size_t n = write(logline, len);
		size_t remain = len - n;
		while (remain > 0) {
			size_t x = write(logline + n, remain);
			if (x == 0) {
				int err = ferror(fp_);
				if (err) {
					fprintf(stderr, "LogFile::File::append() failed %s\n", ::strerror_tl(err));
				}
				break;
			}
			n += x;
			remain = len - n; // remain -= x
		}
		writtenBytes_ += len;
	}

	void Flush() {
		::fflush(fp_);
	}

	size_t GetWrittenBytes() const { return writtenBytes_; }

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

LogFile::LogFile(const string& basename,
	size_t rollSize,
	bool threadSafe,
	int flushInterval)
	: basename_(basename)
	, rollSize_(rollSize)
	, flushInterval_(flushInterval)
	, count_(0)
	, mutex_(threadSafe ? new Mutex : NULL)
	, startOfPeriod_(0)
	, lastRoll_(0)
	, lastFlush_(0) {
	assert(basename.find('/') == string::npos);
	rollFile();
}

LogFile::~LogFile() {
}

void LogFile::Append(const char* logline, int len) {
	if (mutex_) {
		MutexGuard lock(*mutex_);
		append_unlocked(logline, len);
	} else {
		append_unlocked(logline, len);
	}
}

void LogFile::Flush() {
	if (mutex_) {
		MutexGuard lock(*mutex_);
		file_->Flush();
	} else {
		file_->Flush();
	}
}

void LogFile::append_unlocked(const char* logline, int len) {
	file_->Append(logline, len);
	if (file_->GetWrittenBytes() > rollSize_) {
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
				file_->Flush();
			}
		} else {
			++count_;
		}
	}
}

void LogFile::rollFile() {
	time_t now = 0;
	string filename = getLogFileName(basename_, &now);
	time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = start;
		file_.reset(new File(filename));
	}
}

string LogFile::getLogFileName(const string& basename, time_t* now) {
	string filename;
	filename.reserve(basename.size() + 64);
	filename = basename;

	char timebuf[32];
	char pidbuf[32];
	struct tm tm;
	*now = ::time(NULL);
	::gmtime_r(now, &tm); // FIXME: localtime_r ?
	::strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
	filename += timebuf;
	//filename += ProcessInfo::hostname();
	//snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
	//filename += pidbuf;
	filename += ".log";

	return filename;
}