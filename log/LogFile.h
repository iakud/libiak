#ifndef IAK_LOG_LOGFILE_H
#define IAK_LOG_LOGFILE_H

#include "Logging.h" // strerror_tl

#include <assert.h>
#include <stdio.h>
#include <time.h>

namespace iak {

class LogFile {

public:
	explicit LogFile(const std::string& filename)
		: fp_(::fopen(filename.data(), "ae"))
		, writtenBytes_(0) {
		assert(fp_);
		::setbuffer(fp_, buffer_, sizeof buffer_);
		// posix_fadvise POSIX_FADV_DONTNEED ?
	}

	~LogFile() {
		::fclose(fp_);
	}
	// noncopyable
	LogFile(const LogFile&) = delete;
	LogFile& operator=(const LogFile&) = delete;

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
	char buffer_[64 * 1024];
	size_t writtenBytes_;
}; // end class LogFile

} // end namespace iak

#endif  // IAK_LOG_LOGFILE_H
