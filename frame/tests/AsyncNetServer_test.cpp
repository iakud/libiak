#ifndef TOT_ASYNCNETSERVER_TEST_H
#define TOT_ASYNCNETSERVER_TEST_H

#include "AsyncNet.h"
#include <iostream>
#include <string>
#include "Thread.h"
using namespace std;

class AsyncServer : public AsyncNet
{
public:
	AsyncServer() : AsyncNet(2)
	{
		InetAddress localAddr(9999,"0.0.0.0");
		m_server = Listen(localAddr, std::bind(&AsyncServer::onEstablish, this, std::placeholders::_1));
		cout<<"server run"<<endl;
	}
	virtual ~AsyncServer()
	{

	}

	bool onEstablish(DataSocketPtr datasock)
	{
		datasock->SetConnectCallback(std::bind(&AsyncServer::onConnect, this, std::placeholders::_1));
		datasock->SetMessageCallback(std::bind(&AsyncServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
		datasock->SetDisconnectCallback(std::bind(&AsyncServer::onDisconnect, this, std::placeholders::_1));
		return true;
	}

	void onConnect(DataSocketPtr datasock)
	{
		cout<<"client connect"<<endl;
	}

	void onMessage(DataSocketPtr datasock, PacketPtr packet)
	{
		string msg(packet->GetData(), packet->GetDataSize());
		cout<<"client message:"<<msg<<endl;
		datasock->Send(packet);
	}

	void onDisconnect(DataSocketPtr datasock)
	{
		cout<<"client connect"<<endl;
	}
private:
	TcpServerPtr m_server;
};

bool bquit = false;
void processnet(AsyncServer* server)
{
	while (!bquit)
	{
		server->NetProcess();
	}
}

int main()
{
	AsyncServer myserver;
	Thread* thread = new Thread(std::bind(&processnet, &myserver));
	string msg;
	cin>>msg;
	while (msg != "exit")
	{
		cin>>msg;
	}
	bquit = true;
	thread->Join();
}

#endif // TOT_ASYNCNETSERVER_TEST_H

