#ifndef TOT_TCPSERVER_TEST_H
#define TOT_TCPSERVER_TEST_H

#include <net/EventLoop.h>
#include <net/TcpServer.h>
#include <net/TcpConnection.h>

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

void onConnection(TcpConnectionPtr connection) {
	cout<<"accept new client"<<endl;
	connection->setReceiveCallback(std::bind(onReceive, std::placeholders::_1, std::placeholders::_2));
	connection->setDisconnectCallback(std::bind(onDisconnect, std::placeholders::_1));
	connection->establishAsync();
}

int main() {
	EventLoop* loop = new EventLoop();
	InetAddress localAddr(9999,"0.0.0.0");
	TcpServerPtr tcpserver = TcpServer::make(loop, localAddr);
	tcpserver->setConnectCallback(std::bind(onConnection, std::placeholders::_1));
	tcpserver->listenAsync();
	cout<<"tcpserver listen"<<endl;
	loop->loop();
	delete loop;
	return 0;
}

#endif // TOT_TCPSERVER_TEST_H

