#ifndef IAK_LOG_LOGGING_H
#define IAK_LOG_LOGGING_H

#include <string>

namespace iak {

class Logging {

public:
	inline static void setLogDestination(const std::string& destination) {
		if (destination.find_last_of('/') != destination.length()) {
			s_destination_ = destination + '/';
		}
		else {
			s_destination_ = destination;
		}
	}

	inline static const std::string& getLogDestination() {
		return s_destination_;
	}

private:
	// static LogLevel s_level_;
	static std::string s_destination_;
}; // end class Logging

const char* strerror_tl(int savedErrno);

} // end namespace iak

#endif  // IAK_LOG_LOGGING_H

