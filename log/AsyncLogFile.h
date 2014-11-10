#ifndef IAK_BASE_ASYNCLOGGER_H
#define IAK_BASE_ASYNCLOGGER_H

#include "LogFile.h"

#include <base/NonCopyable.h>
#include <base/Mutex.h>

#include <string>
#include <memory>

namespace iak {

class AsyncLogger;

class AsyncLogFile : public LogFile {

public:
	static LogFilePtr make(AsyncLogger* asyncLogger,
			const std::string& basename,
			size_t rollSize,
			int flushInterval = 3);

public:
	AsyncLogFile(AsyncLog* asyncLog,
			const std::string& basename,
			size_t rollSize,
			int flushInterval);
	~AsyncLogFile();

	virtual void append(const char* logline, int len);
	virtual void flush();

protected:
	virtual void append_unlocked(const char* logline, int len) {
	virtual void flush_unlocked();

	AsyncLogger* asyncLogger_;

	class Buffer;
	typedef std::shared_ptr<Buffer> BufferPtr;

	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	std::vector<BufferPtr> buffers_;

	friend class AsyncLogger;
}; // end class LogFile

} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGER_H
