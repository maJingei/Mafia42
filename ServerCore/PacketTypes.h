#pragma once

// ��Ŷ�� �����Դϴ�. �α��� �Ҷ� ä�ú����� ���..
enum EPACKET_TYPE
{
	LOGIN, // �α��� �� ��
	GAMESTART, // ���� ���۵� ��
	CHAT, // ä�õ�
	VOTE, // ��ǥ
	GRANTROLE, // ���� �ο�
	TIME, // �ð� ��Ŷ
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
