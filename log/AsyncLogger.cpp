#include "LogFile.h"
#include "Logger.h" // strerror_tl
#include "AsyncLogging.h"

#include <base/ProcessInfo.h>

#include <assert.h>

using namespace iak;

std::string LogFile::s_dir_ = "./";
bool LogFile::s_host_ = true;
bool LogFile::s_pid_ = true;

void LogFile::setLogDir(const std::string& dir) {
	if (dir.find_last_of('/') != dir.length()) {
		s_dir_ = dir + '/';
	} else {
		s_dir_ = dir;
	}
}

void LogFile::setHostInLogFileName(bool host) {
	s_host_ = host;
}

void LogFile::setPidInLogFileName(bool pid) {
	s_pid_ = pid;
}

LogFilePtr LogFile::make(const std::string& basename,
		size_t rollSize,
		bool threadSafe,
		int flushInterval) {
	return std::make_shared<LogFile>(nullptr, basename, 
			rollSize, threadSafe, flushInterval);
}

LogFilePtr LogFile::make(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize) {
	return std::make_shared<LogFile>(asyncLogging, basename,
			rollSize, false, 0);
}

LogFile::LogFile(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize,
		bool threadSafe,
		int flushInterval)
	: asyncLogging_(asyncLogging)
	, basename_(basename)
	, rollSize_(rollSize)
	, flushInterval_(flushInterval)
	, count_(0)
	, mutexPtr_(threadSafe ? new std::mutex : nullptr)
	, startOfPeriod_(0)
	, lastRoll_(0)
	, lastFlush_(0) {
	assert(basename.find('/') == std::string::npos);
	rollFile();
}

LogFile::~LogFile() {
}

void LogFile::append(const char* logline, int len) {
	if (asyncLogging_ == nullptr) {
		if (mutexPtr_) {
			std::unique_lock<std::mutex> lock(*mutexPtr_);
			append_unlocked(logline, len);
		} else {
			append_unlocked(logline, len);
		}
	} else {
		asyncLogging_->append(std::bind(&LogFile::append_async,
				shared_from_this(), std::string(logline, len)));
	}
}

void LogFile::flush() {
	if (asyncLogging_ == nullptr) {
		if (mutexPtr_) {
			std::unique_lock<std::mutex> lock(*mutexPtr_);
			file_->flush();
		} else {
			file_->flush();
		}
	}
}

void LogFile::append_unlocked(const char* logline, int len) {
	file_->append(logline, len);
	if (rollSize_ > 0 && file_->writtenBytes() > rollSize_) {
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

void LogFile::append_async(const std::string& logline) {
	file_->append(logline.c_str(), logline.length());
	if (file_->writtenBytes() > rollSize_) {
		rollFile();
	} else {
		if (count_ > kCheckTimeRoll_) {
			count_ = 0;
			time_t now = ::time(NULL);
			time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
			if (thisPeriod_ != startOfPeriod_) {
				rollFile();
			} else {
				file_->flush();
			}
		} else {
			++count_;
			file_->flush();
		}
	}
}

void LogFile::rollFile() {
	time_t now = 0;
	std::string filename = s_dir_ + getLogFileName(basename_, &now);
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
	struct tm tm;
	*now = ::time(NULL);
	::localtime_r(now, &tm);
	::strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
	filename += timebuf;
	if (s_host_) {
		filename += ".";
		filename += ProcessInfo::hostName();
	}
	if (s_pid_) {
		char pidbuf[32];
		::snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
		filename += pidbuf;
	}
	filename += ".log";

	return filename;
}