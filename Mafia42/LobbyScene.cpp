#include "pch.h"
#include "ResourceManager.h"
#include "BackGround.h"
#include "InputManager.h"
#include "ClientIocpManager.h"
#include "LobbyScene.h"

LobbyScene::LobbyScene()
{
}

LobbyScene::~LobbyScene()
{
	if (hEditHandle)
	{
		::DestroyWindow(hEditHandle);
		hEditHandle = NULL;
	}

	for (UI* item : UIObjects)
	{
		if (item)
		{
			delete item;
			item = nullptr;
		}
	}

	UIObjects.clear();
}

void LobbyScene::Init(HWND hwnd)
{
	Super::Init(hwnd);

	// 이미지부터 Load
	GET_SINGLE(ResourceManager)->LoadTexture(L"LobbyBackGround", L"Sprite\\Map\\LobbyBackGround.bmp");

	GET_SINGLE(ResourceManager)->CreateSprite(L"LobbyBackGround", GET_SINGLE(ResourceManager)->GetTexture(L"LobbyBackGround"));

	{
		BackGround* backGround = new BackGround();
		backGround->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"LobbyBackGround"));
		
		UIObjects.push_back(backGround);
	}

	// Window Edit 창 생성
	hEditHandle = CreateWindowEx(
		0,
		L"EDIT", // 윈도우창에는 다양한 클래스가 존재하는데, ms문서를 살펴보자. EDIT은 채팅창 ㅇㅇ
		NULL, // 생성할 윈도우 창 이름
		WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		// WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, // 창 스타일과 편집 컨트롤. ms 문서 살펴보기
		50, // 채팅 창의 X좌표
		700, // 채팅박스의 Y좌표
		450, // 채팅박스의 너비
		20, // 채팅박스의 높이
		_hwnd,
		(HMENU)ID_EDIT,
		NULL,
		NULL);

	for (UI* item : UIObjects)
	{
		item->Init();
	}
}

void LobbyScene::Update()
{
	if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::Enter))
	{
		::GetWindowText(hEditHandle, RecvBuffer, BUFSIZE);

		// 채팅 입력한 버퍼에 글자가 적혀있어야됨
		if (wcscmp(RecvBuffer, L"") != 0)
		{
			// TODO : 글자 적혀있으면 서버에 패킷 만들어서 전송
		}
	}
	for (UI* item : UIObjects)
	{
		item->Update();
	}
}

void LobbyScene::Render(HDC hdc)
{
	if (hEditHandle)
	{
		// ::InvalidateRect(hEditHandle, NULL, FALSE);
		::RedrawWindow(hEditHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	for (UI* item : UIObjects)
	{
		item->Render(hdc);
	}
}
