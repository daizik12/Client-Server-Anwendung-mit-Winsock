#include "Server.h"

int Server::Pack_Send(SOCKET& Current, Type_Pack& Type, std::string& temp_buf)
{
	if (send(Current, (char*)&Type, sizeof(Type_Pack), NULL) == SOCKET_ERROR)
	{
		std::cout << "Error Type Packet sending\n";
		return -1;
	}
	switch (Type)
	{
	case messege:
		{
		size_t size_pack = temp_buf.size();
			if (send(Current, (char*)&size_pack, sizeof(size_t), NULL) == SOCKET_ERROR)
			{
				std::cout << "Error size Packet sending\n";
				return -1;
			}
			if (send(Current, temp_buf.c_str(), size_pack, NULL) == SOCKET_ERROR) {
				std::cout << "Error Packet sending\n";
					return -1;
			}
			break;
		}
	default:	{
		std::cout << "Error: Package type is not defined\n";
		return -1;
		}
	}
	temp_buf.clear();
	return 0;
}
int Server::Pack_Recv(SOCKET& Current, Type_Pack& Type, std::string& temp_buf)
{
	if (recv(Current, (char*)&Type, sizeof(Type_Pack), NULL) == SOCKET_ERROR)
	{
		std::cout << "Error Type Packet recv\n";
		return -1;
	}
	switch (Type)
	{
	case messege:{
			size_t size_pack;
			if (recv(Current, (char*)&size_pack, sizeof(size_t), NULL) == SOCKET_ERROR)
			{
				std::cout << "Error size Packet recv\n";
				return -1;
			}
			char* msg = new char[size_pack + 1];
			if (recv(Current, msg, size_pack, NULL) == SOCKET_ERROR) {
				std::cout << "Error Packet recv\n";
				return -1;
			}
			temp_buf.append(msg, size_pack);
			break;
		}
	default: {
			std::cout << "Error: Package type is not defined\n";
			return -1;
		}
	}
	return 0;
}


int Server::Connection(std::vector<SOCKET>& CurrentCon) {
	int sizeaddr = sizeof(addr);
	switch (CurrentCon.size())
	{
	case 0: {
		CurrentCon.resize(1);
		CurrentCon[0] = accept(sListen, (SOCKADDR*)&addr, &sizeaddr);
		if (CurrentCon[0] == 0) {
				closesocket(CurrentCon[0]);
				std::cout << "Socket connection error\n";
				return -1;
			}
		break;
		}
		default:
		{
			CurrentCon.push_back(accept(sListen, (SOCKADDR*)&addr, &sizeaddr));
			if (CurrentCon.back() == 0) {
				closesocket(CurrentCon[0]);
				std::cout << "Socket connection error\n";
				return -1;
			}
			break;
		}
	
	}
	std::cout << "Socket connected successfully\n";
	Type_Pack P = messege;
	send(CurrentCon.back(), (char*)&P, sizeof(Type_Pack), NULL);
	std::string msg = "Hello, welcone to my server!\n";
	size_t size_msg = msg.size();
	send(CurrentCon.back(), (char*)&size_msg, sizeof(size_t), NULL);
	send(CurrentCon.back(), msg.c_str(), msg.size(), NULL);
	return 0;
}


void Server::Handler() {
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	fd_set fd_in, fd_out;
	DWORD total;
	Pack_Status Stat = Send;
	while (true) {
		FD_ZERO(&fd_in);
		FD_ZERO(&fd_out);
		FD_SET(sListen, &fd_in);
		for (size_t i = 0; i < Connections.size(); i++) {
			if (Stat == Recv)
				FD_SET(Connections[i], &fd_out);
			else {
				FD_SET(Connections[i], &fd_in);
			}
		}

		if (total = select(0, &fd_in, &fd_out, NULL, &tv) != SOCKET_ERROR) {
			if (FD_ISSET(sListen, &fd_in)) {
				if (Connections.size() < MaxSize)
					Connection(Connections);
			}
		}

		for (size_t i = 0; i < Connections.size() && total > 0; i++) {
			if (FD_ISSET(Connections[i], &fd_in)) {
				if (Pack_Recv(Connections[i], TPack, buf) < 0) {
					std::cout << "Error: Disconnet Socket:" << Connections[i] << '\n';
					closesocket(Connections[i]);
					if (Connections.size() == 1) {
						Connections.clear();
					}
					else
					for (size_t j = i; j < Connections.size()-1; j++) {
						Connections[j] = Connections[j+1];
						Connections.resize(Connections.size() - 1); 
					}
					
					total--;
					break;
				}
				total--;
				Stat = Recv;
			}

			if (FD_ISSET(Connections[i], &fd_out)) {
				if (Pack_Send(Connections[i], TPack, buf) < 0) {
					std::cout << "Error: Disconnet Socket:" << Connections[i] << '\n';
					closesocket(Connections[i]);
					if (Connections.size() == 1) {
						Connections.clear();
					}
					else
					for (size_t j = i; j < Connections.size() - 1; j++) {
						Connections[j] = Connections[j + 1];
						Connections.resize(Connections.size() - 1);

					}
					total--;
					break;
				}
				total--;
				Stat = Send;
			}
		}
	}
}

Server::Server(char i_addr[], int i_port, size_t i_max)
{
	MaxSize = i_max;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(i_addr);
	addr.sin_port = htons(i_port);
	if (WSAStartup(DLLVersion, &WData)) {
		Server::~Server();
		exit(1);
	}
	sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, MaxSize);
	std::cout << "Start Listening\n";
}

Server::~Server()
{
	WSACleanup();
	Server::buf.clear();
	Connections.clear();
}