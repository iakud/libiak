#ifndef TOT_TCPCLIENT_TEST_H
#define TOT_TCPCLIENT_TEST_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "tot.h"

using namespace std;

void onDisconnect(TcpConnectionPtr connection)
{
	cout<<"on disconnect"<<endl;
}

void onReceive(TcpConnectionPtr connection, PacketPtr packet)
{
	string msg(packet->GetData(), packet->GetDataSize());
	cout<<"receive:"<<msg<<endl;
	connection->Send(packet);
}

void onConnect(TcpConnectionPtr connection)
{
	cout<<"on connect"<<endl;
	char data[] = "hello world!";
	PacketPtr packet = Packet::Create();
	memcpy((char*)packet->GetData(), data, strlen(data));
	packet->SetDataSize(strlen(data));
	connection->Send(packet);
	cout<<"send:"<<data<<endl;
}

void onConnection(TcpConnectionPtr connection)
{
	cout<<"connect to server"<<endl;
	connection->SetConnectCallback(std::bind(onConnect, std::placeholders::_1));
	connection->SetReceiveCallback(std::bind(onReceive, std::placeholders::_1, std::placeholders::_2));
	connection->SetDisconnectCallback(std::bind(onDisconnect, std::placeholders::_1));
	connection->EstablishAsync();
}

int main()
{
	EventLoop* loop = EventLoop::Create();
	InetAddress remoteAddr(9999,"127.0.0.1");
	TcpClientPtr tcpclient = TcpClient::Create(loop, remoteAddr);
	tcpclient->SetConnectCallback(std::bind(onConnection, std::placeholders::_1));
	tcpclient->ConnectAsync();
	cout<<"tcpclient connect"<<endl;
	loop->Loop();

	return 0;
}

#endif // TOT_TCPCLIENT_TEST_H

