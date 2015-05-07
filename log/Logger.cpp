#include "Logger.h"
#include "LogFile.h"

#include <base/ProcessInfo.h>

#include <assert.h>

using namespace iak;

std::string Logger::s_dir_ = "./";
bool Logger::s_host_ = true;
bool Logger::s_pid_ = true;

void Logger::setLogDir(const std::string& dir) {
	if (dir.find_last_of('/') != dir.length()) {
		s_dir_ = dir + '/';
	} else {
		s_dir_ = dir;
	}
}

void Logger::setHostInLogFileName(bool host) {
	s_host_ = host;
}

void Logger::setPidInLogFileName(bool pid) {
	s_pid_ = pid;
}

LoggerPtr Logger::make(const std::string& basename,
		size_t rollSize,
		bool threadSafe,
		int flushInterval) {
	return std::make_shared<Logger>(basename,
			rollSize, threadSafe, flushInterval);
}

Logger::Logger(const std::string& basename,
		size_t rollSize,
		bool threadSafe,
		int flushInterval)
	: basename_(basename)
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

Logger::~Logger() {
}

void Logger::append(const char* logLine, int len) {
	if (mutexPtr_) {
		std::unique_lock<std::mutex> lock(*mutexPtr_);
		append_unlocked(logLine, len);
	} else {
		append_unlocked(logLine, len);
	}
}

void Logger::flush() {
	if (mutexPtr_) {
		std::unique_lock<std::mutex> lock(*mutexPtr_);
		logFile_->flush();
	} else {
		logFile_->flush();
	}
}

void Logger::append_unlocked(const char* logLine, int len) {
	logFile_->append(logLine, len);
	if (rollSize_ > 0 && logFile_->writtenBytes() > rollSize_) {
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
				logFile_->flush();
			}
		} else {
			++count_;
		}
	}
}

void Logger::rollFile() {
	time_t now = 0;
	std::string filename = s_dir_ + getLogFileName(basename_, &now);
	time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = start;
		logFile_.reset(new LogFile(filename));
	}
}

std::string Logger::getLogFileName(const std::string& basename, time_t* now) {
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
