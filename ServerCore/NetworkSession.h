#pragma once

struct FPacket;

// Overlapped�� �����Դϴ�.
enum EIO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
};

struct FOverlappedEx
{
	// ĳ���� offset���� ���� Overlapped ������ ����ü�� ���� ���� 
	WSAOVERLAPPED Overlapped = {};

	// IO_TYPE
	int32 Type = 0;
	
	// ��ü�� �����ϱ� ���� Object ����. �� Session�� �����ڿ��� this �����ͷ� �ʱ�ȭ�����ݴϴ�.
	void* Object = nullptr; 
};

class NetworkSession
{
public:
	NetworkSession();
	virtual ~NetworkSession();

	virtual bool Begin();
	virtual bool End();


	virtual bool Bind(SOCKADDR_IN address);
	virtual bool Listen(USHORT port, int32 backlog);
	virtual bool Connect(SOCKADDR_IN address);
	virtual bool Accept(SOCKET listenSocket);
	virtual bool Recv();
	virtual bool Send(FPacket* packet);

	virtual SOCKET GetSocket();
	virtual char* GetBuf();

	bool TCPCreateSocket();

protected:
	// Ŭ���̾�Ʈ�� ����� �����Դϴ�.
	SOCKET ConnSocket;

	// Overlapped ����ü
	FOverlappedEx ReadOverlapped = {};
	FOverlappedEx WriteOverlapped = {};
	FOverlappedEx AcceptOverlapped = {};

	// Packet
	FPacket* ConnPacket;
	// ��Ŷ�� ���� ��ȣ
	DWORD PacketNumber = 0;
	// ��Ŷ�� ����
	DWORD Protocol = EPACKET_TYPE::NONE;

	// Buffer
	char recvBuffer[BUFSIZE] = {};

	// �� ��Ŷ ���� -> ���޵Ǵ� ��Ŷ��� ������ ��� �������� ����
	DWORD DataSize = 0;	
};

