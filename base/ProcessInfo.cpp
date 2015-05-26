#include "ProcessInfo.h"
#include "FileUtil.h"

#include <algorithm>

#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

namespace iak {
namespace base {

std::chrono::system_clock::time_point g_startTime = std::chrono::system_clock::now();

__thread int t_numOpenedFiles = 0;
int fdDirFilter(const struct dirent* d) {
	if (::isdigit(d->d_name[0])) {
		++t_numOpenedFiles;
	}
	return 0;
}

__thread std::vector<pid_t>* t_pids = NULL;
int taskDirFilter(const struct dirent* d) {
	if (::isdigit(d->d_name[0])) {
		t_pids->push_back(atoi(d->d_name));
	}
	return 0;
}

int scanDir(const char* dirpath, int (*filter)(const struct dirent*)) {
	struct dirent** namelist = NULL;
	int result = ::scandir(dirpath, &namelist, filter, alphasort);
	assert(namelist == NULL);
	return result;
}

} // end namespace base
} // end namespace iak

using namespace iak::base;

pid_t ProcessInfo::pid() {
	return ::getpid();
}

std::string ProcessInfo::pidString() {
	char buf[32];
	::snprintf(buf, sizeof buf, "%d", pid());
	return buf;
}

uid_t ProcessInfo::uid() {
	return ::getuid();
}

std::string ProcessInfo::userName() {
	struct passwd pwd;
	struct passwd* result = NULL;
	char buf[8192];
	const char* name = "unknownuser";

	::getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
	if (result) {
		name = pwd.pw_name;
	}
	return name;
}

uid_t ProcessInfo::euid() {
	return ::geteuid();
}

std::chrono::system_clock::time_point ProcessInfo::startTime() {
	return g_startTime;
}

std::string ProcessInfo::hostName() {
	char buf[64] = "unknownhost";
	buf[sizeof buf - 1] = '\0';
	::gethostname(buf, sizeof buf);
	return buf;
}

std::string ProcessInfo::procName() {
	std::string name;
	std::string stat = procStat();
	size_t lp = stat.find('(');
	size_t rp = stat.find(')');
	if (lp != std::string::npos && rp != std::string::npos) {
		name = stat.substr(lp + 1, rp - lp - 1);
	}
	return name;
}

std::string ProcessInfo::procStatus() {
	std::string result;
	FileReader::readFile("/proc/self/status", result, 65536);
	return result;
}

std::string ProcessInfo::procStat() {
	std::string result;
	FileReader::readFile("/proc/self/stat", result, 65536);
	return result;
}

std::string ProcessInfo::exePath() {
	std::string result;
	char buf[1024];
	ssize_t nRead = ::readlink("/proc/self/exe", buf, sizeof buf);
	if (nRead>0) {
		result.assign(buf, nRead);
	}
	return result;
}

int ProcessInfo::openedFiles() {
	t_numOpenedFiles = 0;
	scanDir("/proc/self/fd", fdDirFilter);
	return t_numOpenedFiles;
}

int ProcessInfo::maxOpenFiles() {
	struct rlimit rl;
	if (::getrlimit(RLIMIT_NOFILE, &rl)) {
		return openedFiles();
	} else {
		return static_cast<int>(rl.rlim_cur);
	}
}

int ProcessInfo::numThreads() {
	int result = 0;
	std::string status = procStatus();
	size_t pos = status.find("Threads:");
	if (pos != std::string::npos) {
		result = ::atoi(status.c_str() + pos + 8);
	}
	return result;
}

std::vector<pid_t> ProcessInfo::threads() {
	std::vector<pid_t> result;
	t_pids = &result;
	scanDir("/proc/self/task", taskDirFilter);
	t_pids = NULL;
	std::sort(result.begin(), result.end());
	return result;
}
