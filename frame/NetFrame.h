#ifndef IAK_FRAME_NETFRAME_H
#define IAK_FRAME_NETFRAME_H

namespace iak {

class NetFrame {
public:
	// int eventloop count
	static void init(uint32_t count);
	static void destroy();
	// run in main thread, per loop
	static void dispatch();
}; // end class NetFrame

} // end namespace iak

#endif // IAK_FRAME_NETFRAME_H