#include "Logging.h"

#include <string.h>

namespace iak {
namespace log {

thread_local char t_errnobuf[512];

const char* strerror_tl(int savedErrno) {
	return ::strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

/*
LogLevel GetLogLevel() {
	if (::getenv("IAK_LOG_TRACE"))
		return LogLevel::TRACE;
	else if (::getenv("IAK_LOG_DEBUG"))
		return LogLevel::DEBUG;
	else
		return LogLevel::INFO;
}

LogLevel Logging::s_level_ = GetLogLevel();
*/
} // end namespace log
} // end namespace iak

using namespace iak::log;

std::string Logging::s_destination_("./");