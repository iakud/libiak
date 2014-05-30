#ifndef IAK_BASE_LOGFORMAT_H
#define IAK_BASE_LOGFORMAT_H

#include "LogStream.h"

namespace iak {

class LogFormat {
public:
	template<typename T>
	Format(const char* fmt, T val);

	const char* GetData() const { return data_; }
	int GetLength() const { return length_; }

private:
	char* data_[32];
	int length_;
}; // end class LogFormat

inline LogStream& operator<<(LogStream& stream, const LogFormat& fmt) {
	stream.Append(fmt.GetData(), fmt.GetLength());
	return stream; 
}

} // end namespace iak

#endif // IAK_LOG_LOGFORMAT_H
