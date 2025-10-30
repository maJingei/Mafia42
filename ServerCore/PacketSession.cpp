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

	// ���� ���� ���� �о��ְ�
	ConnPacket->header.packetLength = 0;
	ConnPacket->header.packetNum = 0;
	ConnPacket->header.protocol = EPACKET_TYPE::NONE;
	ZeroMemory(ConnPacket->ConnectBuffer, sizeof(ConnPacket->ConnectBuffer));
	DataSize = 0;


	// ��Ŷ �ѹ� ������Ű�� (TODO : ���߿��� �� PacketNumber�� ��Ŷ�� ��ȿ���� �˻��� �� �ְڽ��ϴ�.) 
	PacketNumber++;

	// �����ʹ� ����(4byte) | packetNum(4byte) | protocol(4byte) | Data(~) �� �����Ǿ� �ֽ��ϴ�.
	DataSize = sizeof(FPacketHeader) + dataLen;
	ConnPacket->header.packetLength = ::htonl(sizeof(FPacketHeader) + dataLen);
	ConnPacket->header.packetNum = ::htonl(PacketNumber);
	ConnPacket->header.protocol = ::htonl(protocol);
	CopyMemory(ConnPacket->ConnectBuffer, packet, dataLen);

	// ������� ��Ŷ �ϼ� !

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
		cout << "ConnectEx �Լ� �޸� �ε� ����" << endl;
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
			return false; // �̹� ����Ǿ� ����
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
