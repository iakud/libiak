#ifndef IAK_BASE_NONCOPYABLE_H
#define IAK_BASE_NONCOPYABLE_H

namespace iak {

class NonCopyable
{
protected:
	NonCopyable() {} 
	~NonCopyable() {}
private:
	NonCopyable( const NonCopyable& );
	const NonCopyable& operator=( const NonCopyable& );
};

} // end namespace iak

#endif // IAK_BASE_NONCOPYABLE_H
