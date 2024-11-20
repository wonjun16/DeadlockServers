#ifdef _DEBUG
#pragma comment(lib, "./mysql-connector/lib64/debug/vs14/mysqlcppconn.lib")
#else
#pragma comment(lib, "./mysql-connector/lib64/vs14/mysqlcppconn.lib")
#endif


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
		sql::Driver* driver = nullptr;
		sql::Connection* connection = nullptr;
		sql::Statement* statement = nullptr;
		sql::ResultSet* resultSet;
		sql::PreparedStatement* preparedStatement = nullptr;

		driver = get_driver_instance();

		connection = driver->connect("tcp://127.0.0.1", "root", "wonjun100%");

		connection->setSchema("member"); //use

		statement = connection->createStatement();

		resultSet = statement->executeQuery("SELECT * FROM user limit 0, 1000;");

		while (resultSet->next())
		{
			cout << resultSet->getInt("no") << " : ";
			cout << resultSet->getString("user_id") << " : ";
			cout << resultSet->getString("user_password") << " : ";
			cout << resultSet->getString("email") << " : ";
			cout << resultSet->getString("created_at") << " : ";
			cout << resultSet->getInt("is_del") << endl;
		}

		char Buffer[1024] = { 0, };
		//string message;

		cin.getline(Buffer, 1024);

		//string Query = "insert into guestbook (`user_no`, `content`, `create_at`, `read_count`) values ";
		//Query = Query + "('5', '" + MultiByteToUtf8(message) + "', '2024-11-19', '0');";
		//statement->execute(Query);
		preparedStatement = connection->prepareStatement("insert into guestbook(`user_no`, `content`, `create_at`, `read_count`) values ( ? , ? , ? , ? )");

		preparedStatement->setInt(1, 5);
		preparedStatement->setString(2, MultiByteToUtf8(Buffer));
		preparedStatement->setDateTime(3, "2024-11-19");
		preparedStatement->setInt(4, 0);
		preparedStatement->execute();

		resultSet = statement->executeQuery("SELECT * FROM guestbook; ");

		while (resultSet->next())
		{
			for (int i = 1; i <= 5; ++i)
			{
				cout << Utf8ToMultiByte(resultSet->getString(i)) << " : ";
			}
			cout << endl;
		}

		delete resultSet;
		resultSet = nullptr;

		delete statement;
		statement = nullptr;

		delete preparedStatement;
		preparedStatement = nullptr;

		delete connection;
		connection = nullptr;

		driver = nullptr;
	}
	catch (sql::SQLException e)
	{
		cout << "FILE :" << __FILE__ << endl;
		cout << "LINE :" << __LINE__ << endl;
		cout << "Error :" << e.what() << endl;
		cout << "MySQL ErrorCode :" << e.getErrorCode() << endl;
		cout << "SQLState :" << e.getSQLState() << endl;
	}


	return 0;
}

/*
* @brief 웹에서 사용하는 utf-8 인코딩을 window에서 사용하는 ANSI 인코딩으로 변경 합니다.
*/
std::string Utf8ToMultiByte(std::string utf8_str)
{
	std::string resultString;
	char* pszIn = new char[utf8_str.length() + 1];
	strncpy_s(pszIn, utf8_str.length() + 1, utf8_str.c_str(), utf8_str.length());

	int nLenOfUni = 0, nLenOfANSI = 0;
	wchar_t* uni_wchar = NULL;
	char* pszOut = NULL;

	// 1. utf8 Length
	if ((nLenOfUni = MultiByteToWideChar(CP_UTF8, 0, pszIn, (int)strlen(pszIn), NULL, 0)) <= 0)
		return 0;

	uni_wchar = new wchar_t[nLenOfUni + 1];
	memset(uni_wchar, 0x00, sizeof(wchar_t) * (nLenOfUni + 1));

	// 2. utf8 --> unicode
	nLenOfUni = MultiByteToWideChar(CP_UTF8, 0, pszIn, (int)strlen(pszIn), uni_wchar, nLenOfUni);

	// 3. ANSI(multibyte) Length
	if ((nLenOfANSI = WideCharToMultiByte(CP_ACP, 0, uni_wchar, nLenOfUni, NULL, 0, NULL, NULL)) <= 0)
	{
		delete[] uni_wchar;
		return 0;
	}

	pszOut = new char[nLenOfANSI + 1];
	memset(pszOut, 0x00, sizeof(char) * (nLenOfANSI + 1));

	// 4. unicode --> ANSI(multibyte)
	nLenOfANSI = WideCharToMultiByte(CP_ACP, 0, uni_wchar, nLenOfUni, pszOut, nLenOfANSI, NULL, NULL);
	pszOut[nLenOfANSI] = 0;
	resultString = pszOut;

	delete[] uni_wchar;
	delete[] pszOut;

	return resultString;
}

/*
* @brief window에서 사용하는 ANSI 인코딩을 웹에서 사용하는 utf-8 인코딩 인코딩으로 변경 합니다.
*/
std::string MultiByteToUtf8(std::string multibyte_str)
{
	char* pszIn = new char[multibyte_str.length() + 1];
	strncpy_s(pszIn, multibyte_str.length() + 1, multibyte_str.c_str(), multibyte_str.length());

	std::string resultString;

	int nLenOfUni = 0, nLenOfUTF = 0;
	wchar_t* uni_wchar = NULL;
	char* pszOut = NULL;

	// 1. ANSI(multibyte) Length
	if ((nLenOfUni = MultiByteToWideChar(CP_ACP, 0, pszIn, (int)strlen(pszIn), NULL, 0)) <= 0)
		return 0;

	uni_wchar = new wchar_t[nLenOfUni + 1];
	memset(uni_wchar, 0x00, sizeof(wchar_t) * (nLenOfUni + 1));

	// 2. ANSI(multibyte) ---> unicode
	nLenOfUni = MultiByteToWideChar(CP_ACP, 0, pszIn, (int)strlen(pszIn), uni_wchar, nLenOfUni);

	// 3. utf8 Length
	if ((nLenOfUTF = WideCharToMultiByte(CP_UTF8, 0, uni_wchar, nLenOfUni, NULL, 0, NULL, NULL)) <= 0)
	{
		delete[] uni_wchar;
		return 0;
	}

	pszOut = new char[nLenOfUTF + 1];
	memset(pszOut, 0, sizeof(char) * (nLenOfUTF + 1));

	// 4. unicode ---> utf8
	nLenOfUTF = WideCharToMultiByte(CP_UTF8, 0, uni_wchar, nLenOfUni, pszOut, nLenOfUTF, NULL, NULL);
	pszOut[nLenOfUTF] = 0;
	resultString = pszOut;

	delete[] uni_wchar;
	delete[] pszOut;

	return resultString;
}