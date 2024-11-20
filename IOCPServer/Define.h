#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#define MAX_SOCKBUF 256
#define MAX_WORKERTHREAD 4

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
	SOCKET m_socketClient;
	OverlappedEx m_stRecvOverlappedEx;
	OverlappedEx m_stSendOverlappedEx;

	char recvBuf[MAX_SOCKBUF];
	char sendBuf[MAX_SOCKBUF];

	ClientInfo()
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(OverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(OverlappedEx));
		m_socketClient = INVALID_SOCKET;
	}
};