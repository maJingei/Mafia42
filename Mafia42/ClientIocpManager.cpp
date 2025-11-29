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
			// 완료된 작업의 타입을 확인합니다.
			switch (overlappedEx->Type)
			{
			case EIO_TYPE::READ:
				HandlePacket(session, bytesTransferred);

				// 처리가 끝났으니 다음 데이터를 받기 위해 다시 Recv를 겁니다.
				if (session->Recv() == false)
				{
					break;
				}
				break;

			case EIO_TYPE::WRITE:
				//// Send가 완료되었습니다. (보통 클라이언트에선 특별히 할 일 없음)
				break;
			}
		}
	}
}

bool ClientIocpManager::RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey)
{
	// 전달된 Socket을 cp에 등록합니다.
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
			break; // 전달된 데이터 크기가 헤더보다 작으면 다 안옴
		}

		// PacketHeader가 Packet의 offset의 맨 위에 있었기 때문에 캐스팅 가능
		FPacketHeader* header = (FPacketHeader*)tempBuffer;
		int32 tempPacketLen = ::ntohl(header->packetLength); // 네트워크 엔디안 맞춰줌

		if (tempPacketLen <= 0)
		{
			break; // packetLen이 0보다 작으면 문제있음
		}

		if (CurrentSize < tempPacketLen)
		{
			// 전달된 Data의 size가 Packet의 Len보다 작으면 문제있음
			break;
		}

		// 여기까지 오면 하나의 패킷은 보장되니 패킷 처리
		ProcessPacket(session, tempBuffer, CurrentSize);

		DWORD remainSize = CurrentSize - tempPacketLen;

		if (remainSize > 0)
		{
			// 남은 크기만큼을 처리한 패킷 후부터 처음으로 옮겨줌
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

	IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); // IOCP 핸들 생성
	if (!IocpHandle)
	{
		return false;
	}

	// Session 생성
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
	case EPACKET_TYPE::GAMESTART: // 게임 시작 메세지 전송하면 WndProc에서 Scene바꿀거임. 
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
