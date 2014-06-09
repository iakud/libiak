#ifndef TOT_ASYNCNETCLIENT_TEST_H
#define TOT_ASYNCNETCLIENT_TEST_H

#include "AsyncNet.h"
#include <iostream>
#include <string>
#include <string.h>
#include "Thread.h"
using namespace std;

class AsyncClient : public AsyncNet
{
public:
	AsyncClient() : AsyncNet(1)
	{
		InetAddress remoteAddr(9999,"127.0.0.1");
		m_client = Connect(remoteAddr, std::bind(&AsyncClient::onEstablish, this, std::placeholders::_1));
		cout<<"client run"<<endl;
	}
	virtual ~AsyncClient()
	{
		
	}

	bool onEstablish(DataSocketPtr datasock)
	{
		datasock->SetConnectCallback(std::bind(&AsyncClient::onConnect, this, std::placeholders::_1));
		datasock->SetMessageCallback(std::bind(&AsyncClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));
		datasock->SetDisconnectCallback(std::bind(&AsyncClient::onDisconnect, this, std::placeholders::_1));
		return true;
	}

	void onConnect(DataSocketPtr datasock)
	{
		cout<<"client connect"<<endl;
		m_datasock = datasock;
	}

	void onMessage(DataSocketPtr datasock, PacketPtr packet)
	{
		string msg(packet->GetData(), packet->GetDataSize());
		cout<<"client message:"<<msg<<endl;
	}

	void onDisconnect(DataSocketPtr datasock)
	{
		cout<<"client connect"<<endl;
		m_datasock.reset();
	}

	void Send(string msg)
	{
		if (m_datasock)
		{
			PacketPtr packet = Packet::Create();
			memcpy((char*)packet->GetData(), msg.c_str(), msg.size());
			packet->SetDataSize(msg.size());
			m_datasock->Send(packet);
		}
	}
private:
	DataSocketPtr m_datasock;
	TcpClientPtr m_client;
};

bool bquit = false;
void processnet(AsyncClient* client)
{
	while (!bquit)
	{
		client->NetProcess();
	}
}

int main()
{
	AsyncClient myclient;
	Thread* thread = new Thread(std::bind(&processnet, &myclient));
	string msg;
	cin>>msg;
	while (msg != "exit")
	{
		myclient.Send(msg);
		cin>>msg;
	}
	bquit = true;
	thread->Join();
}

#endif // TOT_ASYNCNETCLIENT_TEST_H

