#include "AsyncLogger.h"
#include "AsyncLogging.h"
#include "LogFile.h"
#include "LogConfig.h"

#include <base/ProcessInfo.h>

#include <chrono>
#include <assert.h>

using namespace iak;

AsyncLoggerPtr AsyncLogger::make(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize,
		int flushInterval) {
	return std::make_shared<AsyncLogger>(asyncLogging, basename,
			rollSize, flushInterval);
}

AsyncLogger::AsyncLogger(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize,
		int flushInterval)
	: asyncLogging_(asyncLogging)
	, basename_(basename)
	, rollSize_(rollSize)
	, flushInterval_(flushInterval)
	, count_(0)
	, startOfPeriod_(0)
	, lastRoll_(0)
	, lastFlush_(0) {
	assert(basename.find('/') == std::string::npos);
	rollFile();
}

AsyncLogger::~AsyncLogger() {
}

void AsyncLogger::append(const char* logline, int len) {
	
}

void AsyncLogger::flush() {

}

void AsyncLogger::append_unlocked(const char* logline, int len) {
	/*
	logFile_->append(logline, len);
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
			else {
				file_->flush();
			}
		}
		else {
			++count_;
			file_->flush();
		}
	}
	*/
}

void AsyncLogger::rollFile() {
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	if (now > lastRoll_) {
		lastRoll_ = now;
		lastFlush_ = now;
		startOfPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;

		std::string filename;
		const std::string& destination = LogConfig::getLogDestination();
		filename.reserve(destination.size() + basename_.size() + 64);
		filename = destination + basename_;

		struct tm tm_now;
		::localtime_r(&now, &tm_now);
		char timebuf[32];
		::strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm_now);
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