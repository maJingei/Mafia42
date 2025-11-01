#pragma once

// 패킷의 종류입니다. 로그인 할때 채팅보낼때 등등..
enum EPACKET_TYPE
{
	LOGIN, // 로그인 할 때
	GAMESTART, // 게임 시작될 때
	CHAT, // 채팅들
	VOTE, // 투표
	GRANTROLE, // 역할 부여
	TIME, // 시간 패킷
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
	int32 RoleNumber = 0;
};
