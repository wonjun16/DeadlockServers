#include "InitSocket.hpp"
#include <iostream>
#include "mysql.h"
#include "Packet.h"

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN ListenSock;
	InitSockAddr(ListenSock);

	BindSocket(ListenSocket, ListenSock);

	listen(ListenSocket, 10);

	struct timeval TimeOut;
	InitTimeOut(TimeOut, 0, 100);

	fd_set ReadSocketList;
	fd_set CopyReadSocketList;

	FD_ZERO(&ReadSocketList);
	FD_SET(ListenSocket, &ReadSocketList);

	while (true)
	{
		CopyReadSocketList = ReadSocketList;

		int ChangeFDCount = select(0, &CopyReadSocketList, 0, 0, &TimeOut);

		if (ChangeFDCount == 0) continue;
		else if (ChangeFDCount < 0)
		{
			std::cout << "Error : " << GetLastError() << "\n";
			exit(-1);
		}
		else
		{
			for (int i = 0; i < (int)ReadSocketList.fd_count; i++)
			{
				if (FD_ISSET(ReadSocketList.fd_array[i], &CopyReadSocketList))
				{
					//Listen Socket
					if (ReadSocketList.fd_array[i] == ListenSocket)
					{
						SOCKADDR_IN ClientSock;
						SOCKET ClientSocket;
						memset(&ClientSock, 0, sizeof(ClientSock));
						AcceptSocket(ListenSocket, ClientSock, ReadSocketList, ClientSocket);

						std::cout << "Client Socket Accepted\n";
						//char* IP = inet_ntoa(ClientSock.sin_addr);
						
						//send ip, port to client
						Packet packet;
						packet.IP = "127.0.0.1";
						packet.Port = 7777;

						//int SendLength = send(ClientSocket, (char*)&packet, sizeof(packet), 0);
						//std::cout << SendLength << '\n';
					}
					else
					{
						//Client Message
						std::cout << "Recv Client Message\n";
	
						////read
						//Packet Data;
						//int RecvSize = 0;
						//int PacketSize = recv(ReadSocketList.fd_array[i], (char*)&Length, 2, MSG_WAITALL);

						//Length = ntohs(Length);
						//if (Length > 0)
						//{
						//	RecvSize = recv(ReadSocketList.fd_array[i], (char*)&Data, Length, MSG_WAITALL);
						//}

						//if (RecvSize <= 0 || Length <= 0)
						//{
						//	SOCKADDR_IN ClientSock;
						//	memset(&ClientSock, 0, sizeof(ClientSock));
						//	int ClientSockSize = sizeof(ClientSock);

						//	getpeername(ReadSocketList.fd_array[i], (SOCKADDR*)&ClientSock, &ClientSockSize);
						//	char* IP = inet_ntoa(ClientSock.sin_addr);

						//	printf("%s disconnected.\n", IP);

						//	//disconnect
						//	closesocket(ReadSocketList.fd_array[i]);
						//	FD_CLR(ReadSocketList.fd_array[i], &ReadSocketList);

						//}
						//else
						//{
						//	Data.Code = ntohs(Data.Code);
						//	Data.X = ntohs(Data.X);
						//	Data.Y = ntohs(Data.Y);

						//	SetXY(Data);
						//	SetCursor(Data);

						//	Data.Code = htons(Data.Code);
						//	Data.X = htons(Data.X);
						//	Data.Y = htons(Data.Y);
						//	Length = htons(Length);

						//	int SendSize = send(ReadSocketList.fd_array[i], (char*)&Length, sizeof(Length), 0);
						//	SendSize = send(ReadSocketList.fd_array[i], (char*)&Data, sizeof(Data), 0);
					}
				}
			}
		}
	}

	WSACleanup();

	return 0;
}