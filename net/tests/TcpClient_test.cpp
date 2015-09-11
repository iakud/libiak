#ifndef TOT_TCPCLIENT_TEST_H
#define TOT_TCPCLIENT_TEST_H

#include <net/EventLoop.h>
#include <net/TcpClient.h>

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;
using namespace iak::net;

void onDisconnect(TcpConnectionPtr connection) {
	cout<<"on disconnect"<<endl;
}

void onRecv(TcpConnectionPtr connection, uint32_t size) {
	char msg[1024];
	connection->ReadData(msg, size);
	cout<<"recv "<< size << " data : " <<msg<<endl;
	connection->sendPack(msg, size);
}

void onConnect(TcpConnectionPtr connection) {
	cout<<"on connect"<<endl;
	char data[] = "hello world!";
	connection->sendPack(data, strlen(data));
	cout<<"send:"<<data<<endl;
}

void onConnection(TcpConnectionPtr connection) {
	cout<<"connect to server"<<endl;
	connection->setTcpNoDelay(true);
	connection->setConnectCallback(std::bind(onConnect, std::placeholders::_1));
	connection->setRecvCallback(std::bind(onRecv, std::placeholders::_1, std::placeholders::_2));
	connection->setDisconnectCallback(std::bind(onDisconnect, std::placeholders::_1));
	connection->establish();
}

int main() {
	EventLoop* loop = new EventLoop();
	InetAddress peerAddr(9999,"127.0.0.1");
	TcpClientPtr tcpclient = TcpClient::make(loop, peerAddr);
	tcpclient->setConnectCallback(std::bind(onConnection, std::placeholders::_1));
	tcpclient->setRetry(false);
	tcpclient->connect();
	loop->loop();
	delete loop;
	return 0;
}

#endif // TOT_TCPCLIENT_TEST_H

