#pragma once

#pragma comment(lib, "ws2_32")

#include <thread>
#include <vector>

#include "Define.h"

class IOCompletionPort
{
protected:
	SOCKET ListenSocket = INVALID_SOCKET;

	std::vector<ClientInfo> ClientInfos;

	int ClientCnt = 0;

	std::vector<std::thread> IOWorkerThreads;

	std::thread mAccepterThread;

	HANDLE IOCPHandle = INVALID_HANDLE_VALUE;
	
	bool IsWorkerRun = true;
	
	bool IsAccepterRun = true;

	char SocketBuf[1024] = { 0, };

public:
	IOCompletionPort(void){}
	~IOCompletionPort(void)
	{
		WSACleanup();
	}

	bool InitSocket()
	{
		WSADATA wsaData;

		int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (Result != 0)
		{
			printf("WSAStartup error : %d\n", WSAGetLastError());
			return false;
		}

		ListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

		if (ListenSocket == INVALID_SOCKET)
		{
			printf("Create Socket error : %d\n", WSAGetLastError());
			return false;
		}

		printf("Create Listen Socket\n");
		return true;
	}

	bool BindandListen(int BindPort)
	{
		SOCKADDR_IN ServerAddr;
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(BindPort);
		ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		int Result = bind(ListenSocket, (SOCKADDR*)&ServerAddr, sizeof(SOCKADDR_IN));
		if (Result != 0)
		{
			printf("bind error : %d\n", WSAGetLastError());
			return false;
		}

		Result = listen(ListenSocket, 5);
		if (Result != 0)
		{
			printf("Listen error : %d\n", WSAGetLastError());
			return false;
		}

		printf("Listen Success\n");
		return true;
	}

	bool StartServer(const UINT32 maxClientCount)
	{
		CreateClient(maxClientCount);

		IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);
		if (IOCPHandle == NULL)
		{
			printf("CreateIoCompletionPort error : %d\n", WSAGetLastError());
			return false;
		}

		bool Result = CreateWorkerThread();
		if (Result == false)
		{
			return false;
		}

		Result = CreateAccepterThread();
		if (Result = false)
		{
			return false;
		}

		printf("server start\n");
		return true;
	}

	void DestroyThread()
	{
		IsWorkerRun = false;
		CloseHandle(IOCPHandle);

		for (auto& th : IOWorkerThreads)
		{
			if (th.joinable())
			{
				th.join();
			}
		}

		IsAccepterRun = false;
		closesocket(ListenSocket);

		if (mAccepterThread.joinable())
		{
			mAccepterThread.join();
		}
	}
private:
	void CreateClient(const UINT32 maxClientCount)
	{
		for (UINT32 i = 0; i < maxClientCount; i++)
		{
			ClientInfos.emplace_back();
		}
	}

	bool CreateWorkerThread()
	{
		unsigned int uThreadId = 0;

		for (int i = 0; i < MAX_WORKERTHREAD; i++)
		{
			IOWorkerThreads.emplace_back([this]() {WorkerThread(); });
		}

		printf("worker thread begin..\n");
		return true;
	}

	bool CreateAccepterThread()
	{
		mAccepterThread = std::thread([this]() {AccepterThread(); });

		printf("accepter thread begin..\n");
		return true;
	}

	ClientInfo* GetEmptyClientInfo()
	{
		for (auto& client : ClientInfos)
		{
			if (client.m_socketClient == INVALID_SOCKET)
			{
				return &client;
			}
		}

		return nullptr;
	}

	bool BindIOCP(ClientInfo* pClientInfo)
	{
		auto hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->m_socketClient,
			IOCPHandle, (ULONG_PTR)(pClientInfo), 0);

		if (hIOCP == NULL || hIOCP != IOCPHandle)
		{
			printf("error : CreateIoCompletionPort() failed..\n");
			return false;
		}

		return true;
	}

	bool BindRecv(ClientInfo* pClientInfo)
	{
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
		pClientInfo->m_stRecvOverlappedEx.m_wsaBuf.buf = pClientInfo->recvBuf;
		pClientInfo->m_stRecvOverlappedEx.m_eOperation = IOOperation::RECV;

		int Result = WSARecv(pClientInfo->m_socketClient,
			&(pClientInfo->m_stRecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (pClientInfo->m_stRecvOverlappedEx),
			NULL);

		if (Result == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("error : WSARecv() failed : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	bool SendMsg(ClientInfo* pClientInfo, char* pMsg, int nLen)
	{
		DWORD dwRecvNumBytes = 0;

		//CopyMemory(pClientInfo->m_stSendOverlappedEx.m_szBuf, pMsg, nLen);

		pClientInfo->m_stSendOverlappedEx.m_wsaBuf.len = nLen;
		//pClientInfo->m_stSendOverlappedEx.m_wsaBuf.buf = pClientInfo->m_stSendOverlappedEx.m_szBuf;
		pClientInfo->m_stSendOverlappedEx.m_eOperation = IOOperation::SEND;

		int Result = WSASend(pClientInfo->m_socketClient,
			&(pClientInfo->m_stSendOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED) & (pClientInfo->m_stSendOverlappedEx),
			NULL);

		if (Result == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("error WSASend() failed : %d\n", WSAGetLastError());
			return false;
		}
		return true;
	}

	void WorkerThread()
	{
		ClientInfo* pClientInfo = NULL;

		BOOL bSuccess = TRUE;

		DWORD dIOSIZE = 0;

		LPOVERLAPPED lpOverlapped = NULL;
		printf("worker thread begin .......\n");
		while (IsWorkerRun)
		{
			bSuccess = GetQueuedCompletionStatus(IOCPHandle, &dIOSIZE,
				(PULONG_PTR)&pClientInfo, &lpOverlapped, INFINITE);

			if (bSuccess == TRUE && dIOSIZE == 0 && lpOverlapped == NULL)
			{
				IsWorkerRun = false;
				continue;
			}

			if (lpOverlapped == NULL)
			{
				continue;
			}

			if (bSuccess == FALSE || (dIOSIZE == 0 && bSuccess == TRUE))
			{
				printf("Disconnect with client (%d)\n", (int)pClientInfo->m_socketClient);
				CloseSocket(pClientInfo);
				continue;
			}

			OverlappedEx* pOverlappedEx = (OverlappedEx*)lpOverlapped;

			if (IOOperation::RECV == pOverlappedEx->m_eOperation)
			{
				//pClientInfo->recvBuf
				printf("bytes : %d, msg : %s\n", dIOSIZE, pClientInfo->recvBuf);

				//send to client
				

			}
			else if (IOOperation::SEND == pOverlappedEx->m_eOperation)
			{
				//send 작업 뒤처리
			}
			else
			{
				//예외상황
			}
		}
	}

	void AccepterThread()
	{
		SOCKADDR_IN ClientAddr;
		int AddrLen = sizeof(SOCKADDR_IN);

		while (IsAccepterRun)
		{
			ClientInfo* pClientInfo = GetEmptyClientInfo();
			if (pClientInfo == NULL)
			{
				printf("Error : Client Full\n");
				return;
			}

			pClientInfo->m_socketClient = accept(ListenSocket, (SOCKADDR*)&ClientAddr, &AddrLen);
			if (pClientInfo->m_socketClient == INVALID_SOCKET)
			{
				continue;
			}

			bool Result = BindIOCP(pClientInfo);
			if (Result == false)
			{
				return;
			}

			Result = BindRecv(pClientInfo);
			if (Result == false)
			{
				return;
			}

			char clientIP[32] = { 0, };
			inet_ntop(AF_INET, &(ClientAddr.sin_addr), clientIP, 32 - 1);
			printf("client connected : IP(%s) SOCKET(%d)\n", clientIP, (int)pClientInfo->m_socketClient);

			++ClientCnt;
		}
	}

	void CloseSocket(ClientInfo* pClientInfo, bool bIsForce = false)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

		// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
		if (true == bIsForce)
		{
			stLinger.l_onoff = 1;
		}

		//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
		shutdown(pClientInfo->m_socketClient, SD_BOTH);

		//소켓 옵션을 설정한다.
		setsockopt(pClientInfo->m_socketClient, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		closesocket(pClientInfo->m_socketClient);

		pClientInfo->m_socketClient = INVALID_SOCKET;
	}
};