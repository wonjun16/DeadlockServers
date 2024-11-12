#include "InitSocket.hpp"
#include <iostream>

void InitSockAddr(SOCKADDR_IN& SockAddr, const ADDRESS_FAMILY family, const short sddr, const short port)
{
	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = family;
	SockAddr.sin_addr.s_addr = sddr;
	SockAddr.sin_port = htons(port);
}

void BindSocket(SOCKET& socket, SOCKADDR_IN sockAddr)
{
	int Result = bind(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	if (Result == SOCKET_ERROR)
	{
		std::cout << "Bind Error : " << GetLastError() << "\n";
		exit(-1);
	}
}

void InitTimeOut(timeval& timer, const long sec, const long usec)
{
	timer.tv_sec = sec;
	timer.tv_usec = usec;
}

void AcceptSocket(SOCKET& listenSocket, SOCKADDR_IN& clientSock, fd_set& socketList, SOCKET& ClientSocket)
{
	int ClientSockSize = sizeof(clientSock);
	ClientSocket = accept(listenSocket, (SOCKADDR*)&clientSock, &ClientSockSize);
	FD_SET(ClientSocket, &socketList);
}
