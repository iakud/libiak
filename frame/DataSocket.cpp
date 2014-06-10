#include "DataSocket.h"
#include "AsyncNet.h"

#include <net/TcpConnection.h>

using namespace iak;

DataSocket::DataSocket(std::shared_ptr<TcpConnection> connection,
		CloseCallback& cb)
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

DataSocketPtr DataSocket::create(std::shared_ptr<TcpConnection> connection,
		CloseCallback&& cb) {
	return std::make_shared<DataSocket>(connection, cb);
}

void DataSocket::sendPack(PacketPtr packet) {
	connection_->sendPack(packet);
}

void DataSocket::onConnect(std::shared_ptr<TcpConnection> connection) {
	AsyncNet::put(std::bind(&DataSocket::handleConnect, this, connection));
}

void DataSocket::onMessage(std::shared_ptr<TcpConnection> connection,
		PacketPtr packet) {
	AsyncNet::put(std::bind(&DataSocket::handleMessage, this,
				connection, packet));
}

void DataSocket::onDisconnect(std::shared_ptr<TcpConnection> connection) {
	AsyncNet::put(std::bind(&DataSocket::handleDisconnect, this, connection));
}

void DataSocket::handleConnect(std::shared_ptr<TcpConnection> connection) {
	connectCallback_(shared_from_this());
}

void DataSocket::handleMessage(std::shared_ptr<TcpConnection> connection,
		PacketPtr packet) {
	messageCallback_(shared_from_this(), packet);
}

void DataSocket::handleDisconnect(std::shared_ptr<TcpConnection> connection) {
	DataSocketPtr sharedthis = shared_from_this();
	closeCallback_(sharedthis);
	disconnectCallback_(sharedthis);
}
