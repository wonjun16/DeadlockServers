#include "Server.h"

void Server::OnInit()
{
	db = new DB();
}

void Server::OnConnect(ClientInfo* pClientInfo)
{
	//db, json Ã³¸®
	db->ExecuteQuery("select * from servers;");

	db->resultSet->next();

	std::string ip = db->resultSet->getString("ip");
	std::string port = db->resultSet->getString("port");
	std::string player = std::to_string(ClientCnt);
	
	db->ExecuteQuery("select * from users;");

	db->resultSet->next();

	std::string nickname = db->resultSet->getString("nickname");

	std::string ResultJSON = "{ \"ip\" : \"" + ip + "\", \"player\" : " + player + "\", \"nickname\" : " + nickname + " }";
	SendMsg(pClientInfo, (char*)ResultJSON.c_str(), ResultJSON.length());
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
