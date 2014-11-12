#include "AsyncLog.h"
#include "LogFile.h"

#include <base/Timestamp.h>

#include <stdio.h>
#include <memory.h>

using namespace iak;

void Logger::append(const char* logline, int len) {
	if (mutex_) {
		MutexGuard lock(*mutex_);
		append_unlocked(logline, len);
	} else {
		append_unlocked(logline, len);
	}
}

void Logger::flush() {
	if (mutex_) {
		MutexGuard lock(*mutex_);
		file_->flush();
	} else {
		file_->flush();
	}
}