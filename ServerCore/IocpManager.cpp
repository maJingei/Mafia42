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
			// NetworkSession�� Begin���� this�����ͷ� �ʱ�ȭ ������� ������ ������ ĳ����
			// �׷� ���� client�� accept�Ǿ GetQueued�� IOCP���� ������ �Դٰ� ġ��,
			// networkSession���� ĳ������ session�� client���Լ� �� session�̰�, 
			// �� �ȿ� ���ԵǾ��ִ� socket�� clientsocket�� �� �Դϴ�.
			// - AcceptOverlapped ��ü�� client ������ ���� Session�� �Դϴ�.
			PacketSession* session = (PacketSession*)overlappedEx->Object;
			

			switch (overlappedEx->Type)
			{
			case EIO_TYPE::ACCEPT:
			{
				// �׷� ���� Iocp�� ���
				if (!RegisterIocpSocket(session->GetSocket(), (ULONG_PTR)session))
				{
					break;
				}

				// �� ���� ��Ŷ �ؼ��ϰ�, �׿��°� Process���� ����
				HandlePacket(session, bytesTransferred);

				// �׸��� �ٽ� Recv �ɱ�
				if (session->Recv() == false)
				{
					break;
				}
				break;
			}
			case EIO_TYPE::READ:
			{
				// read�� ���� ��Ŷ ó��
				HandlePacket(session, bytesTransferred);

				// �׸���� �ٽ� recv �ϸ� ��
				if (session->Recv() == false)
				{
					break;
				}
				break;
			}
			case EIO_TYPE::WRITE:
				// write ���� ���� �Ұ� ����
				break;
			}
		}
	}
}

bool IocpManager::RegisterIocpSocket(SOCKET socket, ULONG_PTR completionKey)
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

	// session �����Ҵ�
	ListenSession = new PacketSession();

	ListenSession->Begin();

	// IocpHandle Create
	IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!IocpHandle)
	{
		cout << "Iocp Handle Create Failure" << endl;
		return false;
	}

	// session�� socket ����
	if (!ListenSession->TCPCreateSocket())
	{
		return false;
	}

	// session�� socket �ش� ��Ʈ�� bind �� listen ����
	if (!ListenSession->Listen(7777, SOMAXCONN))
	{
		return false;
	}

	// ListenSession�� socket iocp�� ���
	if (!RegisterIocpSocket(ListenSession->GetSocket(), /*key������ Session�Ѱ��ذ���*/(ULONG_PTR)ListenSession))
	{
		return false;
	}

	// Accept�� session 10�������� ���� �����Ҵ��ϰ�, Accept �������صڿ�, ���Ἲ���ϸ� vector�� pushback
	for (int32 i = 0; i < 10; i++)
	{
		PacketSession* AcceptSession = new PacketSession();
		if (AcceptSession->Begin() == false)
		{
			break;
		}

		if (AcceptSession->Accept(ListenSession->GetSocket()) == true)
		{
			// TODO : GQCS�Լ����� �Ѿ�ͼ� ACCEPT�� Ȯ�εǸ� �׶� Session�� ����־�� ��.
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
