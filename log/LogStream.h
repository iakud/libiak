#ifndef IAK_LOG_LOGSTREAM_H
#define IAK_LOG_LOGSTREAM_H

#include <string>
#include <memory.h> // memcpy

namespace iak {
namespace log {

class LogStream {

public:
	LogStream()
		: cur_(data_)
		, end_(data_ + sizeof data_) {
	}
	// noncopyable
	LogStream(const LogStream&) = delete;
	LogStream& operator=(const LogStream&) = delete;

	LogStream& operator<<(short v);
	LogStream& operator<<(unsigned short v);
	LogStream& operator<<(int v);
	LogStream& operator<<(unsigned int v);
	LogStream& operator<<(long v);
	LogStream& operator<<(unsigned long v);
	LogStream& operator<<(long long v);
	LogStream& operator<<(unsigned long long v);

	LogStream& operator<<(float v) { *this << static_cast<double>(v); return *this; }
	LogStream& operator<<(double v);
	LogStream& operator<<(char v) { append(&v, 1); return *this; }
	//LogStream& operator<<(unsigned char v) { Append(&v, 1); return *this; }
	LogStream& operator<<(const char* v) { append(v, strlen(v)); return *this; }
	LogStream& operator<<(const std::string& v) { append(v.c_str(), v.size()); return *this; }
	LogStream& operator<<(bool v) { append(v ? "1" : "0", 1); return *this; }
	LogStream& operator<<(const void*);

	const char* data() const { return data_; }
	int length() const { return static_cast<int>(cur_ - data_); }
	void reset() { cur_ = data_; }

	void append(const char* buf, size_t len) {
		if (static_cast<size_t>(avail()) > len) {
			::memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

private:
	static const int kBufferSize = 4096;
	static const int kMaxNumericSize = 32;

private:
	void static_check(); // for static_assert

	template<typename T>
	void formatInteger(T);

	int avail() const { return static_cast<int>(end_ - cur_); }

	char data_[kBufferSize];
	char* cur_;
	char* end_;
}; // end class LogStream

class LogFormat {
private:
	static const int kBufferSize = 32;

public:
	template<typename T>
	LogFormat(const char* fmt, T val);

	const char* data() const { return data_; }
	int length() const { return length_; }

private:
	char data_[kBufferSize];
	int length_;
}; // end class LogFormat

inline LogStream& operator<<(LogStream& stream, const LogFormat& fmt) {
	stream.append(fmt.data(), fmt.length());
	return stream;
}

} // end namespace log
} // end namespace iak

#endif  // IAK_LOG_LOGSTREAM_H

