#include "Logger.h"
#include "LogFile.h"
#include "Logging.h"

#include <base/ProcessInfo.h>

#include <chrono>
#include <assert.h>

using namespace iak;

LoggerPtr Logger::make(const std::string& basename,
		size_t rollSize,
		bool threadSafe,
		int flushInterval,
		bool hostNameInLogFileName,
		bool pidInLogFileName) {
	return std::make_shared<Logger>(basename, rollSize, threadSafe, 
			flushInterval, hostNameInLogFileName, pidInLogFileName);
}

Logger::Logger(const std::string& basename,
	size_t rollSize,
	bool threadSafe,
	int flushInterval,
	bool hostNameInLogFileName,
	bool pidInLogFileName)
	: basename_(basename)
	, rollSize_(rollSize)
	, flushInterval_(flushInterval)
	, hostNameInLogFileName_(hostNameInLogFileName)
	, pidInLogFileName_(pidInLogFileName)
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
			time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
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
		const std::string& destination = Logging::getLogDestination();
		filename.reserve(destination.size() + basename_.size() + 64);
		filename = destination + basename_;

		struct tm tm_now;
		::localtime_r(&now, &tm_now);
		char timebuf[32];
		::strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm_now);
		filename += timebuf;
		if (hostNameInLogFileName_) {
			filename += ".";
			filename += ProcessInfo::hostName();
		}
		if (pidInLogFileName_) {
			char pidbuf[32];
			::snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
			filename += pidbuf;
		}
		filename += ".log";
		logFile_ = std::make_unique<LogFile>(filename);
	}
}