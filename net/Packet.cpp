#include "Packet.h"

#include<netinet/in.h>

PacketPtr Packet::create() {
	return std::make_shared<Packet>();
}

uint16_t Packet::getSize() {
	return ntohs(*pSize_);
}

void Packet::setDataSize(uint16_t size) {
	size += static_cast<uint8_t>(sizeof(uint32_t));
	size += static_cast<uint16_t>(sizeof(uint16_t));
	*pSize_ = htons(size);
}

uint16_t Packet::getDataSize() {
	return ntohs(*pSize_) - static_cast<uint16_t>(sizeof(uint32_t) + sizeof(uint16_t));
}

void Packet::setCmd(uint16_t cmd) {
	*pCmd_ = htons(cmd);
}

uint16_t Packet::getCmd() {
	return ntohs(*pCmd_);
}