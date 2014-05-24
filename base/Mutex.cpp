#include "Mutex.h"

using namespace iak;

Mutex::Mutex()
{
	::pthread_mutex_init(&m_mutex, NULL);
}

Mutex::~Mutex()
{
	::pthread_mutex_destroy(&m_mutex);
}

void Mutex::Lock()
{
	::pthread_mutex_lock(&m_mutex);
}

bool Mutex::TryLock()
{
	return !(::pthread_mutex_trylock(&m_mutex));// ret : EBUSY
}

void Mutex::Unlock()
{
	::pthread_mutex_unlock(&m_mutex);
}
