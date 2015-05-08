#ifndef IAK_LOG_LOGCONFIG_H
#define IAK_LOG_LOGCONFIG_H

#include <string>

namespace iak {

class LogConfig {

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

	inline static void setHostNameInLogFileName(bool hostNameInLogFileName) {
		s_hostNameInLogFileName_ = hostNameInLogFileName;
	}

	inline static bool isHostNameInLogFileName() {
		return s_hostNameInLogFileName_;
	}

	inline static void setPidInLogFileName(bool pidInLogFileName_) {
		s_pidInLogFileName_ = pidInLogFileName_;
	}

	inline static bool isPidInLogFileName() {
		return s_pidInLogFileName_;
	}

private:
	static std::string s_destination_;
	static bool s_hostNameInLogFileName_;
	static bool s_pidInLogFileName_;
}; // end class LogConfig

} // end namespace iak

#endif  // IAK_LOG_LOGCONFIG_H

