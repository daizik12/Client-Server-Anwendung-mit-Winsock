#include "Client.h"



int Client::Pack_Send(SOCKET& Current, Type_Pack& Type, std::string& temp_buf)
{
	if (send(Current, (char*)&Type, sizeof(Type_Pack), NULL) == SOCKET_ERROR)
	{
		std::cout << "Error Type Packet sending\n";
		return -1;
	}
	switch (Type)
	{
	case message:{
		size_t size_pack = temp_buf.size();
		if (send(Current, (char*)&size_pack, sizeof(size_t), NULL) == SOCKET_ERROR){
			std::cout << "Error size Packet sending\n";
			return -1;
		}
		if (send(Current, temp_buf.c_str(), size_pack, NULL) == SOCKET_ERROR) {
			std::cout << "Error Packet sending\n";
			return -1;
		}
		break;
	}
	default: {
		std::cout << "Error: Package type is not defined\n";
		return -1;
		}
	}
	return 0;
}
int Client::Pack_Recv(SOCKET& Current, Type_Pack& Type, std::string& temp_buf)
{
	if (recv(Current, (char*)&Type, sizeof(Type_Pack), NULL) == SOCKET_ERROR){
		std::cout << "Error Type Packet recv\n";
		return -1;
	}
	switch (Type){
	case message: {
		size_t size_pack;
		if (recv(Current, (char*)&size_pack, sizeof(size_t), NULL) == SOCKET_ERROR){
			std::cout << "Error size Packet recv\n";
			return -1;
		}
		char* msg = new char[size_pack+1];
		if (recv(Current, msg, size_pack, NULL) == SOCKET_ERROR) {
			std::cout << "Error: Packet recv\n";
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


void Client::Handler()
{
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	fd_set fd_in, fd_out;
	DWORD total;
	bool Pro = true;
	std::thread Out([this, &Pro]() {
		
		while (Pro) {
		std::getline(std::cin, SendBuf);
		SendBuf += '\n';
		SendType = message;
		if (Pack_Send(Sock, SendType, SendBuf) < 0) {
			closesocket(Sock);
			Pro = false;
			break;
		}
	}});
	//std::thread In([this]() {
		while (Pro) {
		if (Pack_Recv(Sock, RecvType, RecvBuf) < 0) {
			closesocket(Sock);
			Pro = false;
			//Out.~thread();
			break;
		}
		std::cout << RecvBuf;
		
		RecvBuf.clear();
	}//});

	Out.join();
	//In.join();
	/*while (true) {
		FD_ZERO(&fd_in);
		FD_ZERO(&fd_out);

		if (Stat == Recv)
			FD_SET(Sock, &fd_out);
		else {
			FD_SET(Sock, &fd_in);
		}
		select(0, &fd_in, &fd_out, NULL, &tv);
		if (FD_ISSET(Sock, &fd_in)) {
			if (Pack_Recv(Sock, TPack, buf) < 0) {
				closesocket(Sock);
				Client::~Client();
				exit(1);
			}
			Stat = Recv;
		}
		if (FD_ISSET(Sock, &fd_in)) {
			if (Pack_Send(Sock, TPack, buf) < 0) {
				closesocket(Sock);
				Client::~Client();
				exit(1);
			}
			Stat = Send;
		}
	}
	*/
}
	Client::Client(char i_addr[], int i_port) {
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(i_addr);
		addr.sin_port = htons(i_port);
		if (WSAStartup(DLLVersion, &WData)) {
			Client::~Client();
			exit(1);
		}
		Sock = socket(AF_INET, SOCK_STREAM, NULL);
		if (connect(Sock, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
			std::cout << "Socket error, then connected to server.\n";
			Client::~Client();
			exit(1);
		}
		Type_Pack P = message;
		if (Pack_Recv(Sock, P, RecvBuf) < 0) {
			std::cout << "Error: Pakcet recv\n";
			closesocket(Sock);
			Client::~Client();
			exit(1);
		}
	}