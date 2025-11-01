#pragma once

class PacketSesion;

class IocpManager
{
public:
	IocpManager();
	virtual ~IocpManager();

	virtual void WorkerThread();

	virtual bool Begin();
	virtual bool End();

	bool RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey);
	virtual void HandlePacket(PacketSession* session, DWORD byteTrasnferred);
	virtual void ProcessPacket(PacketSession* session, char* Buffer, DWORD CurrentLength);

	void Broadcast(PacketSession* session, FPacket* packet);

	PacketSession* GetSession();


protected:
	// Listen만을 위한 Session입니다.
	PacketSession* ListenSession;

	// Client들이 연결될 Session들의 모음입니다.
	unordered_map<PacketSession*, FClientInfo> mClientSessions;

	// IocpHandle입니다.
	HANDLE IocpHandle = {};

	HANDLE StartEventHandle = {};
};

