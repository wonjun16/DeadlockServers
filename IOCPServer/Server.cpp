#include "Server.h"

void Server::OnConnect(ClientInfo* pClientInfo)
{
}

void Server::OnReceive(ClientInfo* pClientInfo)
{
}

void Server::Run(const UINT32 maxClient)
{
	StartServer(maxClient);
}

void Server::End()
{
	DestroyThread();
}
