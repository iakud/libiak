#include <frame/NetFrame.h>
#include <frame/ListenSocket.h>
#include <iostream>
#include <string>
#include <base/Thread.h>
using namespace std;
using namespace iak;

class AsyncServer {
public:
	AsyncServer() {
		InetAddress localAddr(9999,"0.0.0.0");
		listen_ = ListenSocket::create(localAddr);
		listen_->setConnectCallback(std::bind(&AsyncServer::onAccept, 
					this, std::placeholders::_1));
		listen_->listen();
		cout<<"server run"<<endl;
	}

	~AsyncServer(){
	}

	bool onAccept(DataSocketPtr datasock) {
		datasock->setConnectCallback(std::bind(&AsyncServer::onConnect, 
					this, std::placeholders::_1));
		datasock->setMessageCallback(std::bind(&AsyncServer::onMessage,
					this, std::placeholders::_1, std::placeholders::_2));
		datasock->setDisconnectCallback(std::bind(&AsyncServer::onDisconnect,
					this, std::placeholders::_1));
		cout<<"onAccept"<<endl;
		return true;
	}

	void onConnect(DataSocketPtr datasock) {
		std::shared_ptr<int> ud = std::make_shared<int>(1);
		datasock->setUserData(ud);
		cout<<"onConnect"<<endl;
	}

	void onMessage(DataSocketPtr datasock, PacketPtr packet) {
		std::shared_ptr<int> ud = static_pointer_cast<int>(datasock->getUserData());
		string msg(packet->getData(), packet->getDataSize());
		cout<<"message:"<<msg<<endl;
		datasock->sendPack(packet);
	}

	void onDisconnect(DataSocketPtr datasock) {
		cout<<"onDisconnect"<<endl;
	}
private:
	std::shared_ptr<ListenSocket> listen_;
};

bool bquit = false;
void processnet() {
	while (!bquit) {
		NetFrame::dispatch();
	}
}

int main() {
	NetFrame::init(4);
	AsyncServer myserver;
	Thread* thread = new Thread(std::bind(&processnet));
	thread->start();
	string msg;
	cin>>msg;
	while (msg != "exit") {
		cin>>msg;
	}
	bquit = true;
	thread->join();
}
