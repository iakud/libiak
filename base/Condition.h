#ifndef IAK_BASE_CONDITION_H
#define IAK_BASE_CONDITION_H

#include "NonCopyable.h"

namespace iak {

class Condition : public NonCopyable {
public:
	explicit Condition(Mutex& mutex);
	~Condition();

	void Signal();
	void Broadcast();
	void Wait();
	bool TimedWait(uint64_t timeout);
private:
	Mutex& m_mutex;
	pthread_cond_t m_cond;
};

} // end namespace iak

#endif // IAK_BASE_CONDITION_H
