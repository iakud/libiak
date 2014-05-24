#ifndef IAK_BASE_MUTEX_H
#define IAK_BASE_MUTEX_H

#include <pthread.h>
#include "NonCopyable.h"

namespace iak {

class Mutex : public NonCopyable
{
public:
	explicit Mutex();
	~Mutex();

	void Lock();
	bool TryLock();
	void Unlock();

	pthread_mutex_t* GetPthreadMutex()
	{ return &m_mutex; }
private:
	pthread_mutex_t m_mutex;
};

} // end namespace iak

#endif // IAK_BASE_MUTEX_H
