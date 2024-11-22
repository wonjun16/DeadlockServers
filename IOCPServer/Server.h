#pragma once
#include "IOCP.h"
#include "DB.h"
#include "DataManager.h"

class Server : public IOCP
{
public:
	DB* db;

	Server() = default;
	virtual ~Server() = default;

	virtual void OnInit() override;

	virtual void OnConnect(ClientInfo* pClientInfo) override;

	virtual void OnReceive(ClientInfo* pClientInfo) override;

	void Run(const UINT32 maxClient);

	void End();
};

