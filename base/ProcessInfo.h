#ifndef IAK_BASE_PROCESSINFO_H
#define IAK_BASE_PROCESSINFO_H

#include "Timestamp.h"
#include <vector>

namespace iak {

class ProcessInfo {
public:
	static pid_t Pid();
	static string PidString();
	static uid_t Uid();
	static string UserName();
	static uid_t Euid();
	static Timestamp StartTime();

	static string HostName();
	static string ProcName();

	static string ProcStatus();
	static string ProcStat();
	static string ExePath();

	static int OpenedFiles();
	static int MaxOpenFiles();

	static NumThreads();
	static std::vector<pid_t> Threads();
}; // end class ProcessInfo

} // end namespace iak

#endif // IAK_BASE_PROCESSINFO_H
