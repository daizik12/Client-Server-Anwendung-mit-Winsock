#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "wsock32.lib")

#pragma warning(disable : 4996)

class Server
{
	enum Type_Pack {
		messege
	};
	enum Pack_Status {
		Recv, 
		Send
	};
	Type_Pack TPack;
	std::string buf;
	

	WSADATA WData;
	WORD DLLVersion = MAKEWORD(2, 2);

	SOCKET sListen;
	SOCKADDR_IN addr;
	size_t MaxSize;

	std::vector<SOCKET> Connections;

public:
	int Connection(std::vector<SOCKET>&);

	int Pack_Recv(SOCKET&, Type_Pack&, std::string&);
	int Pack_Send(SOCKET&, Type_Pack&, std::string&);

	void Handler() ;

	Server(char[], int, size_t);
	~Server()
};

