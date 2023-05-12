#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "wsock32.lib")

#pragma warning(disable : 4996)
#include <thread>

class Client
{
	std::mutex mtx;
	enum Type_Pack {
		message
	};

	Type_Pack RecvType;
	Type_Pack SendType;
	std::string RecvBuf;
	std::string SendBuf;

	WSADATA WData;
	WORD DLLVersion = MAKEWORD(2, 2);

	SOCKET Sock;
	SOCKADDR_IN addr;
public:
	//int Connection(std::vector<SOCKET>&);

	int Pack_Recv(SOCKET&, Type_Pack&, std::string&);
	int Pack_Send(SOCKET&, Type_Pack&, std::string&);

	void Handler();
	Client(char[], int);
	~Client();
};

