#include "Logger.h"
#include "LogFile.h"
#include "LogConfig.h"

#include <base/ProcessInfo.h>

#include <assert.h>

using namespace iak;

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
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;

		std::string filename;
		const std::string& destination = LogConfig::getLogDestination();
		filename.reserve(destination.size() + basename_.size() + 64);
		filename = destination + basename_;

		struct tm tm;
		::localtime_r(&now, &tm);
		char timebuf[32];
		::strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
		filename += timebuf;
		if (LogConfig::isHostNameInLogFileName()) {
			filename += ".";
			filename += ProcessInfo::hostName();
		}
		if (LogConfig::isPidInLogFileName()) {
			char pidbuf[32];
			::snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
			filename += pidbuf;
		}
		filename += ".log";
		logFile_ = std::make_unique<LogFile>(filename);
	}
}