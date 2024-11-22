#pragma once

#ifdef _DEBUG
#pragma comment(lib, "./mysql-connector/lib64/debug/vs14/mysqlcppconn.lib")
#else
#pragma comment(lib, "./mysql-connector/lib64/vs14/mysqlcppconn.lib")
#endif

#include <iostream>
#include <exception>

#include "jdbc/mysql_driver.h"   
#include "jdbc/mysql_connection.h"
#include "jdbc/cppconn/statement.h"
#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/exception.h"

class DB
{
public:
	sql::Driver* driver = nullptr;
	sql::Connection* connection = nullptr;
	sql::Statement* statement = nullptr;
	sql::ResultSet* resultSet = nullptr;
	sql::PreparedStatement* preparedStatement = nullptr;

	DB()
	{
		InitDB();
	};
	~DB() {};

	void InitDB()
	{
		std::cout << "Init DB.." << std::endl;
		try
		{
			driver = get_driver_instance();

			connection = driver->connect("tcp://127.0.0.1", "root", "wonjun100%");

			connection->setSchema("deadlock");

			statement = connection->createStatement();
		}
		catch (sql::SQLException e)
		{
			CatchException(e);
		}
	}

	void ExecuteQuery(const sql::SQLString query)
	{
		try
		{
			resultSet = statement->executeQuery(query);
		}
		catch (sql::SQLException e)
		{
			CatchException(e);
		}
	}

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

	void CatchException(sql::SQLException e)
	{
		std::cout << "FILE :" << __FILE__ << std::endl;
		std::cout << "LINE :" << __LINE__ << std::endl;
		std::cout << "Error :" << e.what() << std::endl;
		std::cout << "MySQL ErrorCode :" << e.getErrorCode() << std::endl;
		std::cout << "SQLState :" << e.getSQLState() << std::endl;
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
};