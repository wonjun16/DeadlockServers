#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

const UINT32 MAX_IO_WORKER_THREAD = 4;
const UINT32 MAX_SOCK_RECVBUF = 256;
const UINT32 MAX_SOCK_SENDBUF = 4096;

enum class IOOperation
{
	RECV,
	SEND
};

struct OverlappedEx
{
	WSAOVERLAPPED m_wasOverlapped;
	SOCKET m_socketClient;
	WSABUF m_wsaBuf;
	IOOperation m_eOperation;
};

struct ClientInfo
{
	int user_no;
	std::string user_name;
	SOCKET m_socketClient;
	OverlappedEx m_stRecvOverlappedEx;
	OverlappedEx m_stSendOverlappedEx;

	char recvBuf[MAX_SOCK_RECVBUF] = { 0, };
	char sendBuf[MAX_SOCK_SENDBUF] = { 0, };

	ClientInfo()
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(OverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(OverlappedEx));
		m_socketClient = INVALID_SOCKET;
	}
};