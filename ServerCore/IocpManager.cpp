#include "pch.h"
#include "IocpManager.h"
#include "ThreadManager.h"


IocpManager::IocpManager()
{
	IocpHandle = NULL;
	StartEventHandle = NULL;
	ListenSession = nullptr;
}

IocpManager::~IocpManager()
{
}

void IocpManager::WorkerThread()
{
	DWORD bytesTransferred = 0;
	FOverlappedEx* overlappedEx = nullptr;
	PacketSession* completionKey = nullptr;


	while (true)
	{
		SetEvent(StartEventHandle);

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

		if (result == TRUE)
		{
			// NetworkSession의 Begin에서 this포인터로 초기화 시켜줬기 때문에 가능한 캐스팅
			// 그럼 만약 client와 accept되어서 GetQueued로 IOCP에게 정보가 왔다고 치면,
			// networkSession으로 캐스팅한 session은 client에게서 온 session이고, 
			// 그 안에 포함되어있는 socket은 clientsocket일 것 입니다.
			// - AcceptOverlapped 자체가 client 연결을 위한 Session꺼 입니다.
			PacketSession* session = (PacketSession*)overlappedEx->Object;
			

			switch (overlappedEx->Type)
			{
			case EIO_TYPE::ACCEPT:
			{
				// 그럼 먼저 Iocp에 등록
				if (!RegisterIocpSocket(session->GetSocket(), (ULONG_PTR)session))
				{
					break;
				}

				// 그 다음 패킷 해석하고, 그에맞게 Process까지 진행
				HandlePacket(session, bytesTransferred);

				// 그리고 다시 Recv 걸기
				if (session->Recv() == false)
				{
					break;
				}
				break;
			}
			case EIO_TYPE::READ:
			{
				// read면 먼저 패킷 처리
				HandlePacket(session, bytesTransferred);

				// 그리고는 다시 recv 하면 끝
				if (session->Recv() == false)
				{
					break;
				}
				break;
			}
			case EIO_TYPE::WRITE:
				// write 때는 딱히 할거 없음
				break;
			}
		}
	}
}

bool IocpManager::RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey)
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

void IocpManager::HandlePacket(PacketSession* session, DWORD byteTrasnferred)
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

void IocpManager::ProcessPacket(PacketSession* session, char* Buffer, DWORD CurrentLength)
{
	FPacketHeader* header = (FPacketHeader*)Buffer;

	DWORD packetProtocol = (EPACKET_TYPE)::ntohl(header->protocol);

	switch (packetProtocol)
	{
	case EPACKET_TYPE::LOGIN:
	{
		FPacket* Packet = (FPacket*)Buffer;

		FClientInfo clientinfo = {};
		char Name[BUFSIZE] = {0,};
		CopyMemory(&clientinfo.NickName, Packet->ConnectBuffer, sizeof(Packet->ConnectBuffer));

		std::cout << "[" << clientinfo.NickName << "]" << "entered room" << endl;

		mClientSessions.insert(std::pair<PacketSession*, FClientInfo>(session, clientinfo));
		break;
	}
	case EPACKET_TYPE::CHAT:
	{
		FPacket* Packet = (FPacket*)Buffer;
		Broadcast(Packet);

		break;
	}
	case EPACKET_TYPE::VOTE:
		// TODO
		break;
	}
}


void IocpManager::Broadcast(FPacket* packet)
{
	const DWORD HeaderSize = sizeof(FPacketHeader);
	DWORD packetLen = ::ntohl(packet->header.packetLength);
	DWORD dataLen = packetLen - HeaderSize;
	DWORD protocol = ::ntohl(packet->header.protocol);
	BYTE* data = (BYTE*)packet->ConnectBuffer;

	for (auto ClientSessionPair : mClientSessions)
	{
		PacketSession* targetSession = ClientSessionPair.first;

		if (targetSession->CreatePacket(protocol, data, dataLen) == false)
		{
			continue;
		}

		if (targetSession->Send(targetSession->GetPacket()) == false)
		{
			continue;
		}
	}
}

PacketSession* IocpManager::GetSession()
{
	return ListenSession;
}

bool IocpManager::Begin()
{
	StartEventHandle = ::CreateEventW(0, FALSE, FALSE, 0);
	if (StartEventHandle == NULL)
	{
		End();

		return false;
	}

	// session 동적할당
	ListenSession = new PacketSession();

	ListenSession->Begin();

	// IocpHandle Create
	IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!IocpHandle)
	{
		cout << "Iocp Handle Create Failure" << endl;
		return false;
	}

	// session의 socket 생성
	if (!ListenSession->TCPCreateSocket())
	{
		return false;
	}

	// session의 socket 해당 포트에 bind 및 listen 시작
	if (!ListenSession->Listen(7777, SOMAXCONN))
	{
		return false;
	}

	// ListenSession의 socket iocp에 등록
	if (!RegisterIocpSocket(ListenSession->GetSocket(), /*key값으로 Session넘겨준거임*/(ULONG_PTR)ListenSession))
	{
		return false;
	}

	// Accept할 session 10개정도만 만들어서 동적할당하고, Accept 연결해준뒤에, 연결성공하면 vector에 pushback
	for (int32 i = 0; i < 10; i++)
	{
		PacketSession* AcceptSession = new PacketSession();
		if (AcceptSession->Begin() == false)
		{
			break;
		}

		if (AcceptSession->Accept(ListenSession->GetSocket()) == true)
		{
			// TODO : GQCS함수에서 넘어와서 ACCEPT가 확인되면 그때 Session에 집어넣어야 함.
			// ClientSessions.push_back(AcceptSession);
		}
	}

	for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([=]()
			{
				IocpManager::WorkerThread();
			});
	}

	GThreadManager->Join();

	return true;
}

bool IocpManager::End()
{
	ListenSession->End();

	for (auto session : mClientSessions)
	{
		session.first->End();
	}

	mClientSessions.clear();

	::CloseHandle(IocpHandle);

	return true;
}
