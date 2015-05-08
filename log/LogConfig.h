#ifndef IAK_LOG_LOGCONFIG_H
#define IAK_LOG_LOGCONFIG_H

#include <string>

namespace iak {

class LogConfig {

public:
	static void setLogDestination(const std::string& destination);
	static const std::string& getLogDestination();
	static void setHostNameInLogFileName(bool host);
	static bool isHostNameInLogFileName();
	static void setPidInLogFileName(bool pid);
	static bool isPidInLogFileName();

private:
	static std::string s_destination_;
	static bool s_hostNameInLogFileName_;
	static bool s_pidInLogFileName_;
}; // end class LogConfig

std::string LogConfig::s_destination_("./");
bool LogConfig::s_hostNameInLogFileName_(true);
bool LogConfig::s_pidInLogFileName_(true);

void LogConfig::setLogDestination(const std::string& destination) {
	if (destination.find_last_of('/') != destination.length()) {
		s_destination_ = destination + '/';
	} else {
		s_destination_ = destination;
	}
}

const std::string& LogConfig::getLogDestination() {
	return s_destination_;
}

void LogConfig::setHostNameInLogFileName(bool hostNameInLogFileName) {
	s_hostNameInLogFileName_ = hostNameInLogFileName;
}

bool LogConfig::isHostNameInLogFileName() {
	return s_hostNameInLogFileName_;
}

void LogConfig::setPidInLogFileName(bool pidInLogFileName_) {
	s_pidInLogFileName_ = pidInLogFileName_;
}

bool LogConfig::isPidInLogFileName() {
	return s_pidInLogFileName_;
}

} // end namespace iak

#endif  // IAK_LOG_LOGSTREAM_H

