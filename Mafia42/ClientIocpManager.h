#pragma once
#include "IocpManager.h"

class ClientIocpManager
{
public:
	DECLARE_SINGLE(ClientIocpManager)
	~ClientIocpManager();

public:
	virtual void WorkerThread() ;
	
	bool RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey);
	virtual void HandlePacket(PacketSession* session, DWORD byteTrasnferred);
	virtual void ProcessPacket(PacketSession* session, char* Buffer, DWORD CurrentLength);

	virtual bool End();
	virtual bool Init(HWND hwnd);

	PacketSession* GetSession();

public:
	// Listen만을 위한 Session입니다.
	PacketSession* ListenSession;

	// IocpHandle입니다.
	HANDLE IocpHandle = {};

	HANDLE StartEventHandle = {};

	WCHAR ClientName[BUFSIZE] = {};

	// PostMessage함수를 위한 hwnd
	HWND _hwnd;
};

