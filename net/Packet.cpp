#include "Packet.h"

#include <netinet/in.h>

using namespace iak;

PacketPtr Packet::make() {
	return std::make_shared<Packet>();
}

uint16_t Packet::getSize() {
	return ntohs(*pSize_);
}

void Packet::setDataSize(uint16_t size) {
	*pSize_ = htons(static_cast<uint16_t>(size 
				+ sizeof(uint32_t) + sizeof(uint16_t)));
}

uint16_t Packet::getDataSize() {
	return static_cast<uint16_t>(ntohs(*pSize_)
			- (sizeof(uint32_t) + sizeof(uint16_t)));
}

void Packet::setCmd(uint16_t cmd) {
	*pCmd_ = htons(cmd);
}

uint16_t Packet::getCmd() {
	return ntohs(*pCmd_);
}
