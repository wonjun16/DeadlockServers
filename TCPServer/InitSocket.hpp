#pragma once

#ifndef _InitSocket_
#define _InitSocket_

#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

void InitSockAddr(SOCKADDR_IN& SockAddr, const ADDRESS_FAMILY family = PF_INET, const short sddr = INADDR_ANY, const short port = 12345);

void BindSocket(SOCKET& socket, SOCKADDR_IN sockAddr);

void InitTimeOut(struct timeval& timer, const long sec, const long usec);

void AcceptSocket(SOCKET& listenSocket, SOCKADDR_IN& clientSock, fd_set& socketList, SOCKET& ClientSocket);

#endif