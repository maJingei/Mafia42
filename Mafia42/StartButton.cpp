#include "pch.h"
#include "SceneManager.h"
#include "ClientIocpManager.h"
#include "StartButton.h"

StartButton::StartButton()
{
}

StartButton::~StartButton()
{
}

void StartButton::Update()
{
	Super::Update();
}

void StartButton::OnClicked()
{
	// ä�� �Է��� ���ۿ� ���ڰ� �����־�ߵ�
	if (wcscmp(GET_SINGLE(ClientIocpManager)->ClientName, L"") == 0)
	{
		// TODO : ���� ���������� ������ ��Ŷ ���� ����
		return;
	}
	GET_SINGLE(SceneManager)->ChangeScene(SceneType::LobbyScene);

	// TODO : Server�� Connect ��û ����

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// TODO : TitleScene������ Edit â �ϳ� ���� �г��� ����������, ClientIocpManager�� �����س���
	// ���⼭ NickName�� ������ �����°ɷ�.
	WCHAR* SendBuffer = GET_SINGLE(ClientIocpManager)->ClientName;
	int32 StrSize = WideCharToMultiByte(CP_ACP, 0, SendBuffer, -1, NULL, 0, NULL, NULL);
	char* charSendBuffer = new char[StrSize];
	WideCharToMultiByte(CP_ACP, 0, SendBuffer, -1, charSendBuffer, StrSize, 0, 0);



	DWORD datalen = 5;
	if (GET_SINGLE(ClientIocpManager)->GetSession()->CreatePacket(EPACKET_TYPE::LOGIN, (BYTE*)charSendBuffer, StrSize) == false)
	{
		return;
		// TODO : ��Ŷ ���� ����
	}

	// ��Ŷ ��������� �����Ҵ��ߴ� char �迭 delete 
	// TODO : ���� �ٵ� WCHAR �̷��� ��ߵǴ°ǰ� ; 
	delete charSendBuffer;
	charSendBuffer = nullptr;

	while (true)
	{
		if (GET_SINGLE(ClientIocpManager)->GetSession()->Connect(serverAddr) == FALSE)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			if (::WSAGetLastError() == WSAEISCONN)
			{
				break;
			}
			break;
		}
	}
}
