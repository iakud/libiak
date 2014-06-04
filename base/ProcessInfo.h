#ifndef IAK_BASE_PROCESSINFO_H
#define IAK_BASE_PROCESSINFO_H

#include "Timestamp.h"
#include <vector>

namespace iak {

class ProcessInfo {
public:
	static pid_t pid();
	static string pidString();
	static uid_t uid();
	static string userName();
	static uid_t euid();
	static Timestamp startTime();

	static string hostName();
	static string procName();

	static string procStatus();
	static string procStat();
	static string exePath();

	static int openedFiles();
	static int maxOpenFiles();

	static numThreads();
	static std::vector<pid_t> threads();
}; // end class ProcessInfo

} // end namespace iak

#endif // IAK_BASE_PROCESSINFO_H
