#include "pch.h"
#include "IocpManager.h"
#include "GameManager.h"
#include "ThreadManager.h"

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
				// Write���� ���� �� �� ����
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

		// �����ϴ� Sessions�� �߰� �س���
		mClientSessions.insert(std::pair<PacketSession*, FClientInfo>(session, clientinfo));

		std::cout << "[" << clientinfo.NickName << "]" << "entered room" << endl;

		
		// TODO : �ϴ� 4������ �ο�����������, �������� �ο� ���̴´�� �����ؼ� �׶� ���� �й��� �� �ֵ��� ����
		if (mClientSessions.size() >= 4)
		{
			// Client�� GAMESTART ��Ŷ�� �ޱ⸸ �ϸ� �Ǳ⶧���� Buffer�� �׳� �ƹ��ų� ��Ƽ� ������
			char SendData[10] = "Play";
			if (session->CreatePacket(EPACKET_TYPE::GAMESTART, (BYTE*)SendData, 5) == false)
			{
				break;
			}

			// 4���� �� �𿴴ٸ�, GameThread�� EventHandle�� Signaled ���·� !
			GET_SINGLE(GameManager)->SetGameEvent();

			ChatBroadcast(session, session->GetPacket());
		}
		else
		{
			// 4���� ��� ���̱� ���� System�޼����� Client�� ���������� �˸�
			ChatBroadcast(session, Packet);

		}
		break;
	}
	case EPACKET_TYPE::CHAT:
	{
		FPacket* Packet = (FPacket*)Buffer;
		ChatBroadcast(session, Packet);

		break;
	}
	case EPACKET_TYPE::VOTE:
		// TODO
		break;
	}
}


void IocpManager::ChatBroadcast(PacketSession* session, FPacket* packet)
{
	const DWORD HeaderSize = sizeof(FPacketHeader);
	DWORD packetLen = ::ntohl(packet->header.packetLength);
	DWORD dataLen = packetLen - HeaderSize;
	DWORD protocol = ::ntohl(packet->header.protocol);

	// ���� Ŭ���̾�Ʈ�� �̸� ã�Ƽ� ����
	std::string SendClientName = {};
	auto it = mClientSessions.find(session);
	if (it != mClientSessions.end())
	{
		SendClientName = it->second.NickName;
	}
	

	for (auto& ClientSessionPair : mClientSessions)
	{
		PacketSession* targetSession = ClientSessionPair.first;
		if (protocol == EPACKET_TYPE::CHAT)
		{
			std::string OriginMessage(packet->ConnectBuffer, dataLen);
			std::string sendMessage = SendClientName + ": " + OriginMessage;

			if (targetSession->CreatePacket(protocol, (BYTE*)sendMessage.c_str(), sendMessage.size() + 1) == false)
			{
				continue;
			}
		}
		else
		{
			BYTE* tempPacket = (BYTE*)packet->ConnectBuffer;
			if (targetSession->CreatePacket(protocol, tempPacket, dataLen) == false)
			{
				continue;
			}
		}

		if (targetSession->Send(targetSession->GetPacket()) == false)
		{
			continue;
		}
	}
}

void IocpManager::TimeBroadcast(const BYTE* data, DWORD datalen)
{
	for (auto& ClientSession : mClientSessions)
	{
		PacketSession* targetSession = ClientSession.first;

		if (targetSession->CreatePacket(EPACKET_TYPE::TIME, data, datalen) == false)
		{
			continue;
		}

		if (targetSession->Send(targetSession->GetPacket()))
		{
			continue;
		}
	}
}

void IocpManager::NetworkShuffleRole()
{
	std::random_device rd;
	std::mt19937 generator(rd());

	// Roles �����ؼ� �������� ����.
	vector<EPlayerRole> Roles{ EPlayerRole::HUMAN, EPlayerRole::HUMAN, EPlayerRole::POLICE, EPlayerRole::MAFIA };
	std::shuffle(Roles.begin(), Roles.end(), generator);
	int32 index = 0;

	for (auto& session : mClientSessions)
	{
		session.second.RoleNumber = Roles[index];
		index++;
	}

	for (auto& session : mClientSessions)
	{
		switch (session.second.RoleNumber)
		{
		case EPlayerRole::HUMAN:
		{
			std::string RoleMessage = "����� �ù� �Դϴ�.";
			if (session.first->CreatePacket(EPACKET_TYPE::CHAT, (BYTE*)RoleMessage.c_str(), RoleMessage.size() + 1) == false)
			{
				break;
			}
			if (session.first->Send(session.first->GetPacket()) == false)
			{
				break;
			}
			break;
		}
		case EPlayerRole::MAFIA:
		{
			std::string RoleMessage = "����� ���Ǿ� �Դϴ�.";
			if (session.first->CreatePacket(EPACKET_TYPE::CHAT, (BYTE*)RoleMessage.c_str(), RoleMessage.size() + 1) == false)
			{
				break;
			}
			if (session.first->Send(session.first->GetPacket()) == false)
			{
				break;
			}
			break;
		}
		case EPlayerRole::POLICE:
		{
			std::string RoleMessage = "����� ���� �Դϴ�.";
			if (session.first->CreatePacket(EPACKET_TYPE::CHAT, (BYTE*)RoleMessage.c_str(), RoleMessage.size() + 1) == false)
			{
				break;
			}
			if (session.first->Send(session.first->GetPacket()) == false)
			{
				break;
			}
			break;
		}
		}
	}
}

PacketSession* IocpManager::GetSession()
{
	return ListenSession;
}

bool IocpManager::Begin()
{
	IocpHandle = NULL;
	StartEventHandle = NULL;
	ListenSession = nullptr;

	cout << "Server Start... " << endl;

	StartEventHandle = ::CreateEventW(0, FALSE, FALSE, 0);
	if (StartEventHandle == NULL)
	{
		End();

		return false;
	}

	// session �����Ҵ�
	ListenSession = new PacketSession();

	ListenSession->Begin();

	cout << "Session Begin ... " << endl;

	// IocpHandle Create
	IocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (!IocpHandle)
	{
		cout << "Iocp Handle Create Failure" << endl;
		return false;
	}

	cout << "IOCP Handle Create ... " << endl;

	// session�� socket ����
	if (!ListenSession->TCPCreateSocket())
	{
		return false;
	}

	cout << "Socket Create ..." << endl;

	// session�� socket �ش� ��Ʈ�� bind �� listen ����
	if (!ListenSession->Listen(7777, SOMAXCONN))
	{
		return false;
	}

	cout << "Start Listen ..." << endl;

	// ListenSession�� socket iocp�� ���
	if (!RegisterIocpSocket(ListenSession->GetSocket(), /*key������ Session�Ѱ��ذ���*/(ULONG_PTR)ListenSession))
	{
		return false;
	}

	cout << "RegisterIOCP of Socket ..." << endl;

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

	// Iocp�� WorkerThread
	GThreadManager->Launch([=]()
		{
			IocpManager::WorkerThread();
		});
	
	// GameThread
	GThreadManager->Launch([]()
		{
			GET_SINGLE(GameManager)->GameThread();
		});

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

	if (IocpHandle)
	{
		::PostQueuedCompletionStatus(IocpHandle, 0, 0, NULL);
	}

	return true;
}
