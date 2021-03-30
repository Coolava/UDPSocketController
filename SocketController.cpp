
#include "SocketController.h"
#include <WS2tcpip.h>
#include <sstream>
SocketController::SocketController()
	:isStartUp_(false)
	, receiveInterval_(100)
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData_) != NO_ERROR)
	{
		return;
	}
	isStartUp_ = true;
}

SocketController::~SocketController()
{
	WSACleanup();
}


UINT_PTR SocketController::connect(UINT portRecv, UINT portTarget, std::string targetAddress, bool nonBlocking)
{
	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_ != INVALID_SOCKET)
	{
		ZeroMemory(&recvAddress_, sizeof(recvAddress_));
		ZeroMemory(&targetAddress_, sizeof(targetAddress_));

		recvAddress_.sin_family = AF_INET;
		recvAddress_.sin_port = htons(portRecv);
		recvAddress_.sin_addr.s_addr = htonl(INADDR_ANY);

		targetAddress_.sin_family = AF_INET;
		targetAddress_.sin_port = htons(portTarget);
		inet_pton(AF_INET, targetAddress.c_str(), &targetAddress_.sin_addr.s_addr);

		bind(socket_, (SOCKADDR*)&recvAddress_, sizeof(recvAddress_));

		if (nonBlocking == true)
		{
			ULONG isNonBlocking = 1;
			ioctlsocket(socket_, FIONBIO, &isNonBlocking); 
		}

		receiver_.setInterval(receiveInterval_);
		receiver_.start(std::bind(&SocketController::receiveProcessor, this));

	}

	return socket_;
}

int SocketController::close()
{
	int rslt = closesocket(socket_);
	receiver_.stop();
	socket_ = INVALID_SOCKET;
	return rslt;
}

int SocketController::send(std::string str)
{
	return sendto(socket_, str.c_str(), str.length(), 0, (SOCKADDR*)&targetAddress_, sizeof(targetAddress_));
}

void SocketController::setInterval(int interval)
{
	receiveInterval_ = interval;
}

void SocketController::receiveProcessor()
{
	int addressSize = sizeof(recvAddress_);

	int received = recvfrom(socket_, buffer, 1024, 0, (SOCKADDR*)&recvAddress_, &addressSize);

	if (received > 0)
	{
		char buf[32];
		inet_ntop(AF_INET, &sendAddress_.sin_addr.s_addr, buf, 32);

		std::wstringstream ss;
		ss << "ip:" << buf << " || " << buffer << "\r\n";

		queueData_.enqueue(ss.str());
	}
	else
	{
		int err = WSAGetLastError();
	}

}

