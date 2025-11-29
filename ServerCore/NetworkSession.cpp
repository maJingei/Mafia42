#include "pch.h"
#include "NetworkSession.h"

NetworkSession::NetworkSession()
{
	// overlapped 구조체 0으로 밀어주기
	ZeroMemory(&ReadOverlapped, sizeof(ReadOverlapped));
	ZeroMemory(&WriteOverlapped, sizeof(WriteOverlapped));
	ZeroMemory(&AcceptOverlapped, sizeof(AcceptOverlapped));

	// Type 초기화
	ReadOverlapped.Type = EIO_TYPE::READ;
	WriteOverlapped.Type = EIO_TYPE::WRITE;
	AcceptOverlapped.Type = EIO_TYPE::ACCEPT;

	// Object에 this포인터를 넣어줌으로서 GQCS 함수에서 반환되는 overlappedEx 구조체에서 session을 뽑아낼 수 있음
	WriteOverlapped.Object = this;
	ReadOverlapped.Object = this;
	AcceptOverlapped.Object = this;

	ConnSocket = NULL;
}

NetworkSession::~NetworkSession()
{
	End();
}

bool NetworkSession::Begin()
{
	// 이미 Socket이 존재하면 실패
	if (ConnSocket)
	{
		cout << "Already Socket Exist" << endl;
		return false;
	}

	// 사용할 패킷 동적할당
	ConnPacket = new FPacket();

	// 패킷 초기화
	ConnPacket->header.packetLength = 0;
	ConnPacket->header.packetNum = 0;
	ConnPacket->header.protocol = EPACKET_TYPE::NONE;
	ZeroMemory(ConnPacket->ConnectBuffer, sizeof(ConnPacket->ConnectBuffer));

	// recvBuffer도 초기화
	ZeroMemory(recvBuffer, sizeof(recvBuffer));

	return true;
}

bool NetworkSession::End()
{
	::closesocket(ConnSocket);

	ConnSocket = INVALID_SOCKET;

	return true;
}

bool NetworkSession::Bind(SOCKADDR_IN address)
{
	// 바인드 실패 시 소켓 닫고 false 반환
	if (::bind(ConnSocket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		cout << "Bind Failed" << endl;
		End();

		return false;
	}

	return true;
}

bool NetworkSession::Listen(USHORT port, int32 backlog)
{
	if (port == 0 || backlog == 0)
	{
		cout << "port or backlog is Not Exist" << endl;
		return false;
	}

	SOCKADDR_IN listenAddr;
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	listenAddr.sin_port = ::htons(port);

	if (::bind(ConnSocket, (SOCKADDR*)&listenAddr, sizeof(listenAddr)) == SOCKET_ERROR)
	{
		cout << "Bind Failed" << endl;
		End();

		return false;
	}

	if (::listen(ConnSocket, backlog) == SOCKET_ERROR)
	{
		cout << "Listen Failed" << endl;
		End();

		return false;
	}

	return true;
}

/*
* 동기 방식의 가장 기본적인 connect함수를 사용한 Connect입니다.
*/
bool NetworkSession::Connect(SOCKADDR_IN address)
{
	if (::connect(ConnSocket, (SOCKADDR*)&address, sizeof(address)) == SOCKET_ERROR)
	{
		cout << "Connect Failed" << endl;
		return false;
	}

	return true;
}

bool NetworkSession::Accept(SOCKET listenSocket)
{
	if (!listenSocket)
	{
		return false;
	}

	if (ConnSocket)
	{
		return false;
	}

	ConnSocket = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ConnSocket == INVALID_SOCKET)
	{
		cout << "소켓 생성 실패" << endl;
		return false;
	}

	LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	DWORD Bytes = 0;

	// WSAIoctl을 사용하여 AcceptEx 함수를 메모리에 로드합니다.
	// WSAIoctl 함수는 중첩된 I/O를 사용할 수 있는 ioctlsocket() 함수의 확장입니다.
	// 함수의 확장판으로 오버랩 I/O를 사용할 수 있습니다. 이 함수의 3번째부터 6번째 매개변수는 입력 및 출력 버퍼로, 여기에 AcceptEx 함수 포인터를 전달합니다.
	// 이는 Mswsock.lib 라이브러리를 참조하지 않고 직접 AcceptEx 함수를 호출할 수 있도록 하기 위한 것입니다.
	int32 result = WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx),
		&Bytes, NULL, NULL);

	if (result == SOCKET_ERROR)
	{
		cout << "AcceptEx 함수 메모리에 로드 실패" << endl;
		End();

		return false;
	}

	// 클라이언트 주소와 서버 주소의 크기만큼은 Buffersize에서 빼줘야 합니다.
	if (lpfnAcceptEx(listenSocket, ConnSocket, recvBuffer, BUFSIZE - ((sizeof(sockaddr) + 16) * 2), sizeof(sockaddr) + 16, sizeof(sockaddr) + 16, &Bytes, &AcceptOverlapped.Overlapped) == FALSE)
	{
		int32 errorCode = WSAGetLastError();

		if (errorCode != ERROR_IO_PENDING)
		{
			End();

			return false;
		}
	}

	return true;
}

bool NetworkSession::Recv()
{
	if (!ConnSocket)
	{
		return false;
	}

	// Before use RecvBuf, Init RecvBuffer
	// ZeroMemory(recvBuffer, BUFSIZE);
	// ZeroMemory(recvBuffer, sizeof(recvBuffer));

	WSABUF wsaBuf;
	wsaBuf.buf = recvBuffer;
	wsaBuf.len = BUFSIZE;

	DWORD recvLen = 0;
	DWORD flags = 0;

	int32 result = ::WSARecv(ConnSocket, &wsaBuf, 1, &recvLen, &flags, &ReadOverlapped.Overlapped, NULL);
	if (result == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();

		if (errorCode != ERROR_IO_PENDING && errorCode != WSAEWOULDBLOCK)
		{
			cout << "Recv Error" << endl;
			End();

			return false;
		}

		return true;
	}


	return true;
}

bool NetworkSession::Send(FPacket* packet)
{
	WSABUF wsaBuf;
	wsaBuf.buf = (char*)packet; // 만든 패킷을 그대로 전달
	wsaBuf.len = DataSize; // PacketHeader까지 포함된 DataSize 전달

	DWORD sendlen = 0;
	DWORD flags = 0;

	int32 result = ::WSASend(ConnSocket, &wsaBuf, 1, &sendlen, flags, &WriteOverlapped.Overlapped, NULL);
	if (result == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();

		if (errorCode != ERROR_IO_PENDING && errorCode != WSAEWOULDBLOCK)
		{
			cout << "Send Error" << endl;
			End();

			return false;
		}

		return true;
	}

	return true;
}

SOCKET NetworkSession::GetSocket()
{
	return ConnSocket;
}

char* NetworkSession::GetBuf()
{
	return recvBuffer;
}

bool NetworkSession::TCPCreateSocket()
{
	if (ConnSocket)
	{
		cout << "Already Socket Exist" << endl;
		return false;
	}

	ConnSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ConnSocket == INVALID_SOCKET)
	{
		cout << "Socket 생성 실패" << endl;
		return false;
	}

	return true;
}
