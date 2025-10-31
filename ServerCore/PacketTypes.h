#pragma once

// ��Ŷ�� �����Դϴ�. �α��� �Ҷ� ä�ú����� ���..
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
