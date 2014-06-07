#include "Packet.h"

#include <netinet/in.h>

PacketPtr Packet::create() {
	return std::make_shared<Packet>();
}

uint16_t Packet::getSize() {
	return ntohs(*pSize_);
}

void Packet::setDataSize(uint16_t size) {
	uint16_t infoLen = sizeof(uint32_t) + sizeof(uint16_t);
	*pSize_ = htons(size + infoLen);
}

uint16_t Packet::getDataSize() {
	uint16_t infoLen = sizeof(uint32_t) + sizeof(uint16_t);
	return ntohs(*pSize_) - infoLen;
}

void Packet::setCmd(uint16_t cmd) {
	*pCmd_ = htons(cmd);
}

uint16_t Packet::getCmd() {
	return ntohs(*pCmd_);
}