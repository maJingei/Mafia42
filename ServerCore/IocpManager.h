#pragma once

class PacketSesion;

class IocpManager
{
public:
	DECLARE_SINGLE(IocpManager)
	virtual ~IocpManager();

	virtual void WorkerThread();

	virtual bool Begin();
	virtual bool End();

	bool RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey);
	virtual void HandlePacket(PacketSession* session, DWORD byteTrasnferred);
	virtual void ProcessPacket(PacketSession* session, char* Buffer, DWORD CurrentLength);

	void ChatBroadcast(PacketSession* session, FPacket* packet);
	void TimeBroadcast(const BYTE* data, DWORD datalen);
	void NetworkShuffleRole();

	PacketSession* GetSession();


protected:
	// Listen���� ���� Session�Դϴ�.
	PacketSession* ListenSession;

	// Client���� ����� Session���� �����Դϴ�.
	unordered_map<PacketSession*, FClientInfo> mClientSessions;

	// IocpHandle�Դϴ�.
	HANDLE IocpHandle = {};

	HANDLE StartEventHandle = {};
};

