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
	// Listen���� ���� Session�Դϴ�.
	PacketSession* ListenSession;

	// IocpHandle�Դϴ�.
	HANDLE IocpHandle = {};

	HANDLE StartEventHandle = {};

	WCHAR ClientName[BUFSIZE] = {};

	// PostMessage�Լ��� ���� hwnd
	HWND _hwnd;
};

