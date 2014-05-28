#ifndef IAK_BASE_SINGLETON_H
#define IAK_BASE_SINGLETON_H

#include "NonCopyable.h"

#include <pthread.h>
#include <stdlib.h> // atexit

namespace iak {

template<typename T>
class Singleton : public NonCopyable {
public:
	static T& Instance() {
		::pthread_once(&s_once, &Singleton::init);
		return *s_value;
	}

	void Destroy() {
		delete s_value;
	}
private:
	Singleton();
	~Singleton();

	static void init() {
		s_value = new T();
		::atexit(destroy);
	}

	static void destroy() {
		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
		T_must_be_complete_type dummy; (void) dummy;
		delete s_value;
	}

	static pthread_once_t s_once;
	static T* s_value;
};

template<typename T>
pthread_once_t Singleton<T>::s_once = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::s_value = NULL;

} // end namespace iak

#endif // IAK_BASE_SINGLETON_H
