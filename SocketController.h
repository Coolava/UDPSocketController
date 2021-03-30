#pragma once
#include <string>
#include <WinSock2.h>
#include "ThreadController/ThreadController.h"
#include "SafeQueue/SafeQueue.h"
#pragma comment(lib, "ws2_32.lib")
class SocketController
{
public:
	SocketController();
	~SocketController();
	UINT_PTR connect(UINT portRecv, UINT portTarget, std::string targetAddress, bool nonBlocking = false);
	int close();

	int send(std::string str);

	auto& getQueue() { return queueData_; }

	/**
	 * @brief Set interval for receive when call connect. Use only non-blocking.
	 * @param interval milliseconds
	*/
	void setInterval(int interval);
private:
	bool isStartUp_;
	UINT port_;

	SOCKET socket_;
	WSADATA wsaData_;
	SOCKADDR_IN recvAddress_, sendAddress_;
	SOCKADDR_IN targetAddress_;
	ThreadController receiver_;
	SafeQueue<std::wstring> queueData_;

	int receiveInterval_;

	char buffer[1024] = { 0, };
	void receiveProcessor();
};

