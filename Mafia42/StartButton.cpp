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
	// 채팅 입력한 버퍼에 글자가 적혀있어야됨
	if (wcscmp(GET_SINGLE(ClientIocpManager)->ClientName, L"") == 0)
	{
		// TODO : 글자 적혀있으면 서버에 패킷 만들어서 전송
		return;
	}
	GET_SINGLE(SceneManager)->ChangeScene(SceneType::LobbyScene);

	// TODO : Server로 Connect 요청 시작

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// TODO : TitleScene에서도 Edit 창 하나 만들어서 닉네임 받은다음에, ClientIocpManager에 저장해놓고
	// 여기서 NickName을 서버로 보내는걸로.
	WCHAR* SendBuffer = GET_SINGLE(ClientIocpManager)->ClientName;
	int32 StrSize = WideCharToMultiByte(CP_ACP, 0, SendBuffer, -1, NULL, 0, NULL, NULL);
	char* charSendBuffer = new char[StrSize];
	WideCharToMultiByte(CP_ACP, 0, SendBuffer, -1, charSendBuffer, StrSize, 0, 0);



	DWORD datalen = 5;
	if (GET_SINGLE(ClientIocpManager)->GetSession()->CreatePacket(EPACKET_TYPE::LOGIN, (BYTE*)charSendBuffer, StrSize) == false)
	{
		return;
		// TODO : 패킷 생성 실패
	}

	// 패킷 만들었으니 동적할당했던 char 배열 delete 
	// TODO : ㅆㅃ 근데 WCHAR 이렇게 써야되는건가 ; 
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
