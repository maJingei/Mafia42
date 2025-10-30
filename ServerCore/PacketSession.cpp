#include "pch.h"
#include "PacketSession.h"

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

bool PacketSession::CreatePacket(DWORD protocol, const BYTE* packet, DWORD dataLen)
{
	if (!packet)
	{
		cout << "Packet Not Exist" << endl;
		return false;
	}

	// 쓰기 전에 먼저 밀어주고
	ConnPacket->header.packetLength = 0;
	ConnPacket->header.packetNum = 0;
	ConnPacket->header.protocol = EPACKET_TYPE::NONE;
	ZeroMemory(ConnPacket->ConnectBuffer, sizeof(ConnPacket->ConnectBuffer));
	DataSize = 0;


	// 패킷 넘버 증가시키고 (TODO : 나중에는 이 PacketNumber로 패킷의 유효성을 검사할 수 있겠습니다.) 
	PacketNumber++;

	// 데이터는 길이(4byte) | packetNum(4byte) | protocol(4byte) | Data(~) 로 구성되어 있습니다.
	DataSize = sizeof(FPacketHeader) + dataLen;
	ConnPacket->header.packetLength = ::htonl(sizeof(FPacketHeader) + dataLen);
	ConnPacket->header.packetNum = ::htonl(PacketNumber);
	ConnPacket->header.protocol = ::htonl(protocol);
	CopyMemory(ConnPacket->ConnectBuffer, packet, dataLen);

	// 여기까지 패킷 완성 !

	return true;
}

bool PacketSession::Connect(SOCKADDR_IN address)
{
	LPFN_CONNECTEX lpfnConnectEx = NULL;
	GUID GuidConnectEx = WSAID_CONNECTEX;
	DWORD Bytes = 0;

	int32 result = WSAIoctl(
		ConnSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&lpfnConnectEx, sizeof(lpfnConnectEx),
		&Bytes, NULL, NULL);

	if (result == SOCKET_ERROR)
	{
		cout << "ConnectEx 함수 메모리 로드 실패" << endl;
		End();
		return false;
	}

	WSABUF wsaBuf;
	wsaBuf.buf = (CHAR*)ConnPacket;
	wsaBuf.len = DataSize;

	if (lpfnConnectEx(ConnSocket, (SOCKADDR*)&address, sizeof(address), wsaBuf.buf, wsaBuf.len, &Bytes, &ReadOverlapped.Overlapped) == FALSE)
	{
		int32 errorCode = ::WSAGetLastError();

		if (errorCode == WSAEISCONN)
		{
			return false; // 이미 연결되어 있음
		}

		if (errorCode != ERROR_IO_PENDING && errorCode != WSAEWOULDBLOCK)
		{
			End();

			return false;
		}
	}

	return true;
}


FPacket* PacketSession::GetPacket()
{
	return ConnPacket;
}

void PacketSession::SetPacket(FPacket* packet)
{
	ConnPacket = packet;
}
DWORD PacketSession::GetDataSize()
{
	return DataSize;
}

void PacketSession::SetDataSize(DWORD dataSize)
{
	DataSize = dataSize;
}
