#ifndef IAK_BASE_PROCESSINFO_H
#define IAK_BASE_PROCESSINFO_H

#include <chrono>
#include <string>
#include <vector>

namespace iak {

class ProcessInfo {
public:
	static pid_t pid();
	static std::string pidString();
	static uid_t uid();
	static std::string userName();
	static uid_t euid();
	static std::chrono::system_clock::time_point startTime();

	static std::string hostName();
	static std::string procName();

	static std::string procStatus();
	static std::string procStat();
	static std::string exePath();

	static int openedFiles();
	static int maxOpenFiles();

	static int numThreads();
	static std::vector<pid_t> threads();
}; // end class ProcessInfo

} // end namespace iak

#endif // IAK_BASE_PROCESSINFO_H
