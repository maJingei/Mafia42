#include "pch.h"
#include "NetworkSession.h"

NetworkSession::NetworkSession()
{
	// overlapped ����ü 0���� �о��ֱ�
	ZeroMemory(&ReadOverlapped, sizeof(ReadOverlapped));
	ZeroMemory(&WriteOverlapped, sizeof(WriteOverlapped));
	ZeroMemory(&AcceptOverlapped, sizeof(AcceptOverlapped));

	// Type �ʱ�ȭ
	ReadOverlapped.Type = EIO_TYPE::READ;
	WriteOverlapped.Type = EIO_TYPE::WRITE;
	AcceptOverlapped.Type = EIO_TYPE::ACCEPT;

	// Object�� this�����͸� �־������μ� GQCS �Լ����� ��ȯ�Ǵ� overlappedEx ����ü���� session�� �̾Ƴ� �� ����
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
	// �̹� Socket�� �����ϸ� ����
	if (ConnSocket)
	{
		cout << "Already Socket Exist" << endl;
		return false;
	}

	// ����� ��Ŷ �����Ҵ�
	ConnPacket = new FPacket();

	// ��Ŷ �ʱ�ȭ
	ConnPacket->header.packetLength = 0;
	ConnPacket->header.packetNum = 0;
	ConnPacket->header.protocol = EPACKET_TYPE::NONE;
	ZeroMemory(ConnPacket->ConnectBuffer, sizeof(ConnPacket->ConnectBuffer));

	// recvBuffer�� �ʱ�ȭ
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
	// ���ε� ���� �� ���� �ݰ� false ��ȯ
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
* ���� ����� ���� �⺻���� connect�Լ��� ����� Connect�Դϴ�.
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
		cout << "���� ���� ����" << endl;
		return false;
	}

	LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	DWORD Bytes = 0;

	// WSAIoctl�� ����Ͽ� AcceptEx �Լ��� �޸𸮿� �ε��մϴ�.
	// WSAIoctl �Լ��� ��ø�� I/O�� ����� �� �ִ� ioctlsocket() �Լ��� Ȯ���Դϴ�.
	// �Լ��� Ȯ�������� ������ I/O�� ����� �� �ֽ��ϴ�. �� �Լ��� 3��°���� 6��° �Ű������� �Է� �� ��� ���۷�, ���⿡ AcceptEx �Լ� �����͸� �����մϴ�.
	// �̴� Mswsock.lib ���̺귯���� �������� �ʰ� ���� AcceptEx �Լ��� ȣ���� �� �ֵ��� �ϱ� ���� ���Դϴ�.
	int32 result = WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx),
		&Bytes, NULL, NULL);

	if (result == SOCKET_ERROR)
	{
		cout << "AcceptEx �Լ� �޸𸮿� �ε� ����" << endl;
		End();

		return false;
	}

	// Ŭ���̾�Ʈ �ּҿ� ���� �ּ��� ũ�⸸ŭ�� Buffersize���� ����� �մϴ�.
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
	wsaBuf.buf = (char*)packet; // ���� ��Ŷ�� �״�� ����
	wsaBuf.len = DataSize; // PacketHeader���� ���Ե� DataSize ����

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
		cout << "Socket ���� ����" << endl;
		return false;
	}

	return true;
}
