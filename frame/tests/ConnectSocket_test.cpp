#include <frame/NetFrame.h>
#include <frame/ConnectSocket.h>
#include <iostream>
#include <string>
#include <string.h>
#include <thread>
using namespace std;
using namespace iak;

class AsyncClient {
public:
	AsyncClient() {
		InetAddress remoteAddr(9999,"127.0.0.1");
		connect_ = ConnectSocket::make(remoteAddr);
		connect_->setConnectCallback(std::bind(&AsyncClient::onConnection, 
					this, std::placeholders::_1));
		connect_->connect();
		cout<<"client run"<<endl;
	}
	virtual ~AsyncClient() {
	}

	bool onConnection(DataSocketPtr datasocket) {
		datasocket->setConnectCallback(std::bind(&AsyncClient::onConnect,
					this, std::placeholders::_1));
		datasocket->setMessageCallback(std::bind(&AsyncClient::onMessage,
					this, std::placeholders::_1, std::placeholders::_2));
		datasocket->setDisconnectCallback(std::bind(&AsyncClient::onDisconnect,
					this, std::placeholders::_1));
		return true;
	}

	void onConnect(DataSocketPtr datasocket) {
		cout<<"client connect"<<endl;
		datasocket_ = datasocket;
	}

	void onMessage(DataSocketPtr datasock, PacketPtr packet) {
		string msg(packet->getData(), packet->getDataSize());
		cout<<"client message:"<<msg<<endl;
	}

	void onDisconnect(DataSocketPtr datasock) {
		cout<<"client connect"<<endl;
		datasocket_.reset();
	}

	void send(string msg) {
		if (datasocket_) {
			PacketPtr packet = Packet::make();
			memcpy(packet->getData(), msg.c_str(), msg.size());
			packet->setDataSize(static_cast<uint16_t>(msg.size()));
			datasocket_->sendPack(packet);
		}
	}
private:
	DataSocketPtr datasocket_;
	std::shared_ptr<ConnectSocket> connect_;
};

bool bquit = false;
void processnet() {
	while (!bquit) {
		NetFrame::dispatch();
	}
}

int main() {
	NetFrame::init(2);
	AsyncClient myclient;
	std::thread t = std::thread(std::bind(&processnet));
	string msg;
	cin>>msg;
	while (msg != "exit") {
		myclient.send(msg);
		cin>>msg;
	}
	bquit = true;
	t.join();
}
