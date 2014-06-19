#ifndef TOT_TCPCLIENT_TEST_H
#define TOT_TCPCLIENT_TEST_H

#include <net/EventLoop.h>
#include <net/TcpClient.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace iak;

void onDisconnect(TcpConnectionPtr connection) {
	cout<<"on disconnect"<<endl;
}

void onReceive(TcpConnectionPtr connection, PacketPtr packet) {
	string msg(packet->getData(), packet->getDataSize());
	cout<<"receive:"<<msg<<endl;
	connection->sendPack(packet);
}

void onConnect(TcpConnectionPtr connection) {
	cout<<"on connect"<<endl;
	char data[] = "hello world!";
	PacketPtr packet = Packet::make();
	memcpy(packet->getData(), data, static_cast<uint16_t>(strlen(data)));
	packet->setDataSize(static_cast<uint16_t>(strlen(data)));
	connection->sendPack(packet);
	cout<<"send:"<<data<<endl;
}

void onConnection(TcpConnectionPtr connection) {
	cout<<"connect to server"<<endl;
	connection->setConnectCallback(std::bind(onConnect, std::placeholders::_1));
	connection->setReceiveCallback(std::bind(onReceive, std::placeholders::_1, std::placeholders::_2));
	connection->setDisconnectCallback(std::bind(onDisconnect, std::placeholders::_1));
	connection->establishAsync();
}

int main() {
	EventLoop* loop = EventLoop::create();
	InetAddress remoteAddr(9999,"127.0.0.1");
	TcpClientPtr tcpclient = TcpClient::make(loop, remoteAddr);
	tcpclient->setConnectCallback(std::bind(onConnection, std::placeholders::_1));
	tcpclient->connectAsync();
	cout<<"tcpclient connect"<<endl;
	loop->loop();

	return 0;
}

#endif // TOT_TCPCLIENT_TEST_H

