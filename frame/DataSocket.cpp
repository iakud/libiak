#include "DataSocket.h"
#include "AsyncNet.h"

#include <net/TcpConnection.h>

using namespace iak;

DataSocketPtr DataSocket::make(TcpConnectionPtr connection,
		CloseCallback&& cb) {
	return std::make_shared<DataSocket>(connection,
			std::forward<CloseCallback>(cb));
}

DataSocket::DataSocket(TcpConnectionPtr connection,
		CloseCallback&& cb)
	: connection_(connection)
	, closeCallback_(cb){
	connection_->setConnectCallback(std::bind(&DataSocket::onConnect,
			this, std::placeholders::_1));
	connection_->setReceiveCallback(std::bind(&DataSocket::onMessage,
			this, std::placeholders::_1, std::placeholders::_2));
	connection_->setDisconnectCallback(std::bind(&DataSocket::onDisconnect,
			this, std::placeholders::_1));
}

DataSocket::~DataSocket() {
}

void DataSocket::close() {
	connection_->closeAsync();
}

void DataSocket::sendPack(PacketPtr packet) {
	connection_->sendPack(packet);
}

void DataSocket::onConnect(TcpConnectionPtr connection) {
	AsyncNet::put(std::bind(&DataSocket::handleConnect, this, connection));
}

void DataSocket::onMessage(TcpConnectionPtr connection,
		PacketPtr packet) {
	AsyncNet::put(std::bind(&DataSocket::handleMessage, this,
				connection, packet));
}

void DataSocket::onDisconnect(TcpConnectionPtr connection) {
	AsyncNet::put(std::bind(&DataSocket::handleDisconnect, this, connection));
}

void DataSocket::handleConnect(TcpConnectionPtr connection) {
	connectCallback_(shared_from_this());
}

void DataSocket::handleMessage(TcpConnectionPtr connection,
		PacketPtr packet) {
	messageCallback_(shared_from_this(), packet);
}

void DataSocket::handleDisconnect(TcpConnectionPtr connection) {
	DataSocketPtr sharedthis = shared_from_this();
	closeCallback_(sharedthis);
	disconnectCallback_(sharedthis);
}
