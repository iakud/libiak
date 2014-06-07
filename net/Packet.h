#ifndef IAK_NET_PACKET_H
#define IAK_NET_PACKET_H

#include <memory>
#include <stdint.h>

namespace iak {

class Packet;
typedef std::shared_ptr<Packet> PacketPtr;

class Packet {
public:
	Packet()
		: pSize_(reinterpret_cast<uint16_t*>(buffer_))
		, pHead_(reinterpret_cast<uint32_t*>(buffer_ + sizeof(uint16_t)))
		, pCmd_(reinterpret_cast<uint16_t*>(buffer_ + sizeof(uint16_t) + sizeof(uint32_t)))
		, pData_(buffer_ + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t)) {
	}

	~Packet() {
	}

	static PacketPtr create();

	const char* getPacket() { return buffer_; }
	char* getData() { return pData_; }

	uint16_t getSize();
	void setDataSize(uint16_t size);
	uint16_t getDataSize();
	void setCmd(uint16_t cmd);
	uint16_t getCmd();

private:
	char buffer_[1024*12];
	uint16_t* pSize_;
	uint32_t* pHead_;
	uint16_t* pCmd_;
	char* pData_;
}; // end class Packet

} // end namespace iak

#endif // IAK_NET_PACKET_H
