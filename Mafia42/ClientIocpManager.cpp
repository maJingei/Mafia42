#include "pch.h"
#include "ClientIocpManager.h"

ClientIocpManager::~ClientIocpManager()
{
}

void ClientIocpManager::WorkerThread()
{
	DWORD bytesTransferred = 0;
	FOverlappedEx* overlappedEx = nullptr;
	PacketSession* completionKey = nullptr;

	while (true)
	{
		BOOL result = ::GetQueuedCompletionStatus(
			IocpHandle,
			&bytesTransferred,
			(ULONG_PTR*)&completionKey,
			(LPOVERLAPPED*)&overlappedEx,
			INFINITE);

		if (!overlappedEx)
		{
			break;
		}

		PacketSession* session = (PacketSession*)overlappedEx->Object;

		if (result == TRUE)
		{
			// �Ϸ�� �۾��� Ÿ���� Ȯ���մϴ�.
			switch (overlappedEx->Type)
			{
			case EIO_TYPE::READ:
				HandlePacket(session, bytesTransferred);

				// ó���� �������� ���� �����͸� �ޱ� ���� �ٽ� Recv�� �̴ϴ�.
				if (session->Recv() == false)
				{
					break;
				}
				break;

			case EIO_TYPE::WRITE:
				//// Send�� �Ϸ�Ǿ����ϴ�. (���� Ŭ���̾�Ʈ���� Ư���� �� �� ����)
				break;
			}
		}
	}
}

bool ClientIocpManager::RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey)
{
	// ���޵� Socket�� cp�� ����մϴ�.
	IocpHandle = CreateIoCompletionPort((HANDLE)socket, IocpHandle, completionKey, 0);

	if (!IocpHandle)
	{
		int32 error = GetLastError();
		if (error == ERROR_INVALID_PARAMETER)
		{
			std::cout << "error parameter" << endl;
		}
		std::cout << "IocpHandle Not Exist" << endl;
		return false;
	}

	return true;
}

void ClientIocpManager::HandlePacket(PacketSession* session, DWORD byteTrasnferred)
{
	char* tempBuffer = session->GetBuf();
	session->SetDataSize(byteTrasnferred);
	const DWORD HeaderSize = sizeof(FPacketHeader);

	while (true)
	{
		DWORD CurrentSize = session->GetDataSize();
		if (CurrentSize <= HeaderSize)
		{
			break; // ���޵� ������ ũ�Ⱑ ������� ������ �� �ȿ�
		}

		// PacketHeader�� Packet�� offset�� �� ���� �־��� ������ ĳ���� ����
		FPacketHeader* header = (FPacketHeader*)tempBuffer;
		int32 tempPacketLen = ::ntohl(header->packetLength); // ��Ʈ��ũ ����� ������

		if (tempPacketLen <= 0)
		{
			break; // packetLen�� 0���� ������ ��������
		}

		if (CurrentSize < tempPacketLen)
		{
			// ���޵� Data�� size�� Packet�� Len���� ������ ��������
			break;
		}

		// ������� ���� �ϳ��� ��Ŷ�� ����Ǵ� ��Ŷ ó��
		ProcessPacket(session, tempBuffer, CurrentSize);

		DWORD remainSize = CurrentSize - tempPacketLen;

		if (remainSize > 0)
		{
			// ���� ũ�⸸ŭ�� ó���� ��Ŷ �ĺ��� ó������ �Ű���
			::memmove(tempBuffer, tempBuffer + tempPacketLen, remainSize);
		}

		session->SetDataSize(remainSize);
	}
}

bool ClientIocpManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
	IocpHandle = NULL;
	StartEventHandle = NULL;
	ListenSession = nullptr;

	StartEventHandle = ::CreateEventW(0, FALSE, FALSE, 0);
	if (StartEventHandle == NULL)
	{
		End();

		return false;
	}

	IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // IOCP �ڵ� ����
	if (!IocpHandle)
	{
		return false;
	}

	// Session ����
	ListenSession = new PacketSession();

	if (ListenSession->Begin() == false)
	{
		return false;
	}

	if (ListenSession->TCPCreateSocket() == false)
	{
		return false;
	}

	SOCKADDR_IN bindAddr;
	::memset(&bindAddr, 0, sizeof(bindAddr));
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bindAddr.sin_port = 0;

	if (ListenSession->Bind(bindAddr) == false)
	{
		return false;
	}

	return true;
}

PacketSession* ClientIocpManager::GetSession()
{
	return ListenSession;
}

void ClientIocpManager::ProcessPacket(PacketSession* session, char* Buffer, DWORD CurrentLength)
{
	FPacketHeader* header = (FPacketHeader*)Buffer;

	DWORD packetProtocol = (EPACKET_TYPE)::ntohl(header->protocol);

	switch (packetProtocol)
	{
	case EPACKET_TYPE::LOGIN:
	{
		FPacket* Packet = (FPacket*)Buffer;

		break; 
	}
	case EPACKET_TYPE::CHAT:
	{
		FPacket* Packet = (FPacket*)Buffer;
		DWORD packetLen = ::ntohl(header->packetLength);
		DWORD dataLen = packetLen - sizeof(FPacketHeader);

		char* chatBuffer = new char[dataLen];
		CopyMemory(chatBuffer, Packet->ConnectBuffer, dataLen);

		::PostMessage(_hwnd, WM_USER_CHAT_RECV, 0, (LPARAM)chatBuffer);
		break;
	}
	case EPACKET_TYPE::GAMESTART: // ���� ���� �޼��� �����ϸ� WndProc���� Scene�ٲܰ���. 
	{
		::PostMessage(_hwnd, WM_USER_GAME_START, 0, 0);
		break;
	}
	case EPACKET_TYPE::GRANTROLE:
	{
		break;
	}
	case EPACKET_TYPE::TIME:
	{
		FPacket* Packet = (FPacket*)Buffer;
		DWORD packetLen = ::ntohl(header->packetLength);
		DWORD dataLen = packetLen - sizeof(FPacketHeader);

		char* chatBuffer = new char[dataLen];
		CopyMemory(chatBuffer, Packet->ConnectBuffer, dataLen);

		::PostMessage(_hwnd, WM_USER_TIME, 0, (LPARAM)chatBuffer);
		break;
	}

	}
}

bool ClientIocpManager::End()
{
	::CloseHandle(IocpHandle);

	if (IocpHandle)
	{
		::PostQueuedCompletionStatus(IocpHandle, 0, 0, NULL);
	}

	return true;
}
