#ifndef IAK_LOG_LOGGER_H
#define IAK_LOG_LOGGER_H

#include <base/NonCopyable.h>
#include <base/BlockingQueue.h>
#include <base/CountDownLatch.h>
#include <base/Mutex.h>
#include <base/Thread.h>

#include <memory>
#include <vector>
#include <string>

namespace iak {

class Logger;
typedef std::shared_ptr<Logger> LoggerPtr;

class Logger : public NonCopyable {
public:
	Logger(const std::string& basename,
		size_t rollSize,
		bool threadSafe = true,
		int flushInterval = 3);

	Logger(AsyncLogging* asyncLogging,
		const std::string& basename,
		size_t rollSize,
		int flushInterval = 3);

	~Logger() {
		
	}

	void append(const char* logline, int len);
	void flush();

private:

	AsyncLogging
	std::unique_ptr<Mutex> mutex_;
};

} // end namespace iak

#endif  // IAK_LOG_LOGGER_H
