#pragma once
#include "NetworkSession.h"

struct FPacket;

class PacketSession : public NetworkSession
{
	using Super = NetworkSession;

public:
	PacketSession();
	~PacketSession();

	bool CreatePacket(DWORD protocol, const BYTE* packet, DWORD dataLen);
	virtual bool Connect(SOCKADDR_IN address) override;

public:
	FPacket* GetPacket();
	void SetPacket(FPacket* packet);
	DWORD GetDataSize();
	void SetDataSize(DWORD dataSize);
};
