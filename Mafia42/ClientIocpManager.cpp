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
				//HandlePacket(session, bytesTransferred);

				//// 처리가 끝났으니 다음 데이터를 받기 위해 다시 Recv를 겁니다.
				//if (session->Recv() == false)
				//{
				//	break;
				//}
				//// Send가 완료되었습니다. (보통 클라이언트에선 특별히 할 일 없음)
				break;
			}
		}
	}
}

bool ClientIocpManager::Init(HWND hwnd)
{
	_hwnd = hwnd;

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
		// TODO : PostMessage 방식을 활용해서 MainThread로 메세지 보내고, WndProc에서 MessageManager가 받아서 PushMessage하기
	}
}
