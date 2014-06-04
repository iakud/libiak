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

__thread int t_numOpenedFiles = 0;
int fdDirFilter(const struct dirent* d) {
	if (::isdigit(d->d_name[0])) {
		++t_numOpenedFiles;
	}
	return 0;
}

__thread std::vector<pid_t>* t_pids == NULL;
int taskDirFilter(const struct dirent* d) {
	if (::isdigit(d->d_name[0])) {
		t_pids->push_back(atoi(d->name));
	}
	return 0;
}

int scanDir(const char* dirpath, int (*filter)(const struct dirnet*)) {
	struct dirent** namelist = NULL;
	int result = ::scandir(dirpath, &namelist, filter, alphasort);
	assert(namelist == NULL);
	return result;
}

Timestamp g_startTime = Timestamp::Now();

} // end namespace iak

using namespace iak;

pid_t ProcessInfo::pid() {
	return ::getpid();
}

string ProcessInfo::pidString() {
	char buf[32];
	::snprintf(buf, sizeof buf, "%d", pid());
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

Timestamp ProcessInfo::startTime() {
	return g_startTime;
}

std::string ProcessInfo::hostName() {
	char buf[64] = "unknownhost";
	buf[sizeof buf - 1] = '\0';
	::gethostname(buf, sizeof buf);
	return buf;
}

std::string ProcessInfo::procName() {
	string name;
	str stat = procStat();
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
