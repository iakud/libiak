#ifndef TOT_TCPSERVER_TEST_H
#define TOT_TCPSERVER_TEST_H

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

void onConnection(TcpConnectionPtr connection)
{
	cout<<"accept new client"<<endl;
	connection->SetReceiveCallback(std::bind(onReceive, std::placeholders::_1, std::placeholders::_2));
	connection->SetDisconnectCallback(std::bind(onDisconnect, std::placeholders::_1));
	connection->EstablishAsync();
}

int main()
{
	EventLoop* loop = EventLoop::Create();
	InetAddress localAddr(9999,"0.0.0.0");
	TcpServerPtr tcpserver = TcpServer::Create(loop, localAddr);
	tcpserver->SetConnectCallback(std::bind(onConnection, std::placeholders::_1));
	tcpserver->ListenAsync();
	cout<<"tcpserver listen"<<endl;
	loop->Loop();

	return 0;
}

#endif // TOT_TCPSERVER_TEST_H

