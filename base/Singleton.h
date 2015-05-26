#ifndef IAK_BASE_SINGLETON_H
#define IAK_BASE_SINGLETON_H

#include <mutex>
#include <stdlib.h> // atexit

namespace iak {
namespace base {

template<typename T>
class Singleton {
public:
	static T& getInstance() {
		std::call_once(s_once_, &Singleton::init);
		return *s_value_;
	}

public:
	// noncopyable
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

private:
	Singleton();
	~Singleton();

	static void init() {
		s_value_ = new T();
		::atexit(destroy);
	}

	static void destroy() {
		typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
		T_must_be_complete_type dummy; (void) dummy;
		delete s_value_;
	}

	static std::once_flag s_once_;
	static T* s_value_;
}; // end class Singleton

template<typename T>
std::once_flag Singleton<T>::s_once_;

template<typename T>
T* Singleton<T>::s_value_ = NULL;

} // end namespace base
} // end namespace iak

#endif // IAK_BASE_SINGLETON_H
