#ifndef IAK_NET_EPOLLPOLLER_H
#define IAK_NET_EPOLLPOLLER_H

struct epoll_event;

namespace iak {
namespace net {

class Channel;

class EPollPoller {
protected:
	static const int kEventSizeInit = 16;
public:
	EPollPoller();
	virtual ~EPollPoller();
	// noncopyable
	EPollPoller(const EPollPoller&) = delete;
	EPollPoller& operator=(const EPollPoller&) = delete;

	void poll(int timeout);
	void addChannel(Channel* channel);
	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);

protected:
	int epollFd_;
	int eventSize_;
	struct epoll_event* events_;
}; // end class EPollPoller

} // end namespace net
} // end namespace iak

#endif // IAK_NET_EPOLLPOLLER_H
