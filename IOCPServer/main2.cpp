#ifdef _DEBUG
#pragma comment(lib, "./mysql-connector/lib64/debug/vs14/mysqlcppconn.lib")
#else
#pragma comment(lib, "./mysql-connector/lib64/vs14/mysqlcppconn.lib")
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <Windows.h>

#include "jdbc/mysql_driver.h"   
#include "jdbc/mysql_connection.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/exception.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

using namespace std;

std::string Utf8ToMultiByte(std::string utf8_str);
std::string MultiByteToUtf8(std::string multibyte_str);

class customexeception : exception
{
public:
	customexeception(string what)
	{
		message = what;
	}

	string message;
};

int main2()
{
	try
	{
		WSAData wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		SOCKADDR_IN ListenSock;
		memset(&ListenSock, 0, sizeof(ListenSock));
		ListenSock.sin_family = AF_INET;
		ListenSock.sin_addr.s_addr = INADDR_ANY;
		ListenSock.sin_port = htons(30000);

		int Result = ::bind(ListenSocket, (SOCKADDR*)&ListenSock, sizeof(ListenSock));

		listen(ListenSocket, 5);

		while (true)
		{
			SOCKADDR_IN ClientSock;
			memset(&ClientSock, 0, sizeof(ClientSock));
			int ClientSockSize = sizeof(ClientSock);
			SOCKET ClientSocket = accept(ListenSocket, (SOCKADDR*)&ClientSock, &ClientSockSize);

			char Buffer[1024] = { 0, };
			int RecvBytes = recv(ClientSocket, Buffer, 1024, 0);


			Document d;
			d.Parse(Buffer);

			Value& s = d["code"];

			if (strcmp("login", s.GetString()) == 0)
			{
				string user_id = d["user_id"].GetString();
				string user_password = d["user_password"].GetString();

				sql::Driver* driver = nullptr;
				sql::Connection* connection = nullptr;
				sql::Statement* statement = nullptr;
				sql::ResultSet* resultSet;
				sql::PreparedStatement* preparedStatement = nullptr;

				driver = get_driver_instance();

				connection = driver->connect("tcp://127.0.0.1", "root", "qweasd123");

				connection->setSchema("member"); //use

				preparedStatement = connection->prepareStatement("select * from user where user_id = ? and user_password = sha(?);");

				preparedStatement->setString(1, user_id);
				preparedStatement->setString(2, user_password);

				resultSet = preparedStatement->executeQuery();

				string ResultJSON = "";
				if (resultSet->rowsCount() == 0)
				{
					//½ÇÆÐ
					ResultJSON = "{ \"result\" : \"FAILED\", \"message\" : \"not found\" }";
				}
				else
				{
					ResultJSON = "{ \"result\" : \"SUCCESSED\", \"message\" : \"found\" }";

				}

				int SendSize = send(ClientSocket, ResultJSON.c_str(), (int)ResultJSON.length(), 0);
			}
			else
			{
				///
			}

			closesocket(ClientSocket);
		}

		closesocket(ListenSocket);
	}
	catch (sql::SQLException e)
	{
		std::cout << "FILE :" << __FILE__ << endl;
		std::cout << "LINE :" << __LINE__ << endl;
		std::cout << "Error :" << e.what() << endl;
		std::cout << "MySQL ErrorCode :" << e.getErrorCode() << endl;
		std::cout << "SQLState :" << e.getSQLState() << endl;
	}




	WSACleanup();


	return 0;
}