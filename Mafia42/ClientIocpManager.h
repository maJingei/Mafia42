#pragma once
#include "IocpManager.h"

class ClientIocpManager : public IocpManager
{
	using Super = IocpManager;
public:
	DECLARE_SINGLE(ClientIocpManager)
	~ClientIocpManager();

public:
	virtual void WorkerThread() override;
	
	virtual bool Begin() override;

	virtual void ProcessPacket(PacketSession* session, char* Buffer, DWORD CurrentLength) override;

public:
	WCHAR ClientName[BUFSIZE] = {};
};

