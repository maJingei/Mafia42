#pragma once

// 패킷의 종류입니다. 로그인 할때 채팅보낼때 등등..
enum EPACKET_TYPE
{
	LOGIN,
	CHAT,
	VOTE,
	NONE,

};

struct FPacketHeader
{
public:
	int32 packetLength = 0;
	DWORD packetNum = 0;
	DWORD protocol = EPACKET_TYPE::NONE;
};

struct FPacket
{
	FPacketHeader header;
	char ConnectBuffer[BUFSIZE] = {};
};

struct FClientInfo
{
	// std::string NickName;
	char NickName[BUFSIZE] = {};
};
