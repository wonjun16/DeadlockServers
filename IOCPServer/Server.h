#pragma once
#include "IOCP.h"

class Server : public IOCP
{
public:
	Server() = default;
	virtual ~Server() = default;

	virtual void OnConnect(ClientInfo* pClientInfo) override;


	virtual void OnReceive(ClientInfo* pClientInfo) override;

	void Run(const UINT32 maxClient);

	void End();
};

