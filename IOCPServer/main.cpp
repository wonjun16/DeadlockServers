#include "Server.h"
#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 12345;
const UINT16 MAX_CLIENT = 100;

int main()
{
	Server server;

	server.InitSocket();

	server.BindandListen(SERVER_PORT);

	server.Run(MAX_CLIENT);

	printf("quit : q\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "q")
		{
			break;
		}
	}

	server.End();

	return 0;
}