#include <base/ProcessInfo.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

int main() {
	printf("pid = %d\n", muduo::ProcessInfo::Pid());
	printf("uid = %d\n", muduo::ProcessInfo::Uid());
	printf("euid = %d\n", muduo::ProcessInfo::Euid());
	printf("start time = %s\n", muduo::ProcessInfo::GetTime().GetString().c_str());
	printf("hostname = %s\n", muduo::ProcessInfo::HostName().c_str());
	printf("opened files = %d\n", muduo::ProcessInfo::OpenedFiles());
	printf("threads = %zd\n", muduo::ProcessInfo::Threads().size());
	printf("num threads = %d\n", muduo::ProcessInfo::NumThreads());
	printf("status = %s\n", muduo::ProcessInfo::ProcStatus().c_str());
}
