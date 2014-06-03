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


}

std::string ProcessInfo::ProcStatus() {
	std::string result;
	FileReader::ReadFile("/proc/self/status", result, 65536);
	return result;
}

std::string ProcessInfo::ProcStat() {
	std::string result;
	FileReader::ReadFile("/proc/self/stat", result, 65536);
	return result;
}

std::string ProcessInfo::ExePath() {
	std::string result;
	char buf[1024];
	ssize_t nRead = ::readlink("/proc/self/exe", buf, sizeof buf);
	if (nRead>0) {
		result.assign(buf, nRead);
	}
	return result;
}
} // end namespace iak
