#pragma once

struct FPacket;

// Overlapped의 종류입니다.
enum EIO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
};

struct FOverlappedEx
{
	// 캐스팅 offset으로 인해 Overlapped 변수는 구조체의 가장 위에 
	WSAOVERLAPPED Overlapped = {};

	// IO_TYPE
	int32 Type = 0;
	
	// 개체를 구분하기 위한 Object 변수. 각 Session의 생성자에서 this 포인터로 초기화시켜줍니다.
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
	// 클라이언트와 연결될 소켓입니다.
	SOCKET ConnSocket;

	// Overlapped 구조체
	FOverlappedEx ReadOverlapped = {};
	FOverlappedEx WriteOverlapped = {};
	FOverlappedEx AcceptOverlapped = {};

	// Packet
	FPacket* ConnPacket;
	// 패킷의 고유 번호
	DWORD PacketNumber = 0;
	// 패킷의 종류
	DWORD Protocol = EPACKET_TYPE::NONE;

	// Buffer
	char recvBuffer[BUFSIZE] = {};

	// 총 패킷 길이 -> 전달되는 패킷헤더 포함한 모든 데이터의 길이
	DWORD DataSize = 0;	
};

