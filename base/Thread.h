#ifndef IAK_BASE_THREAD_H
#define IAK_BASE_THREAD_H

#include "NonCopyable.h"

#include <atomic>
#include <memory>
#include <functional>
#include <stdint.h>
#include <pthread.h>

namespace iak {

extern __thread int t_tid;
extern __thread char t_tidstr[32];
extern __thread const char* t_name;

class Thread : public NonCopyable {
private:
	typedef std::function<void()> ThreadFunc;

public:
	static int GetTid() {
		if (t_tid == 0) {
			cacheTid();
		}
		return t_id;
	}

	static const char* GetTidString() {
		if (t_tid == 0) {
			cacheTid();
		}
		return t_tidstr;
	}

	static const char* GetName() {
		return t_name;
	}

	static bool IsMainThread();
	static void SleepUsec(int64_t usec);
private:
	static void cacheTid();

public:
	explicit Thread(ThreadFunc&& func, const std::string& name);
	~Thread();

	void Start();
	void Join();

private:
	std::atomic_flag started_;
	std::atomic_flag joined_;
	pthread_t thread_;
	std::shared_ptr<int> tid_;
	ThreadFunc func_;
	std::string name_;
};

} // end namespace iak

#endif // IAK_BASE_THREAD_H
