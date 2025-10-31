#include "pch.h"
#include "TitleScene.h"
#include "ResourceManager.h"
#include "TimeManager.h"
#include "Button.h"
#include "StartButton.h"
#include "Sprite.h"
#include "InputManager.h"
#include "ClientIocpManager.h"
#include "BackGround.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
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

void TitleScene::Init(HWND hwnd)
{
	Super::Init(hwnd);
	// 이미지 Load
	GET_SINGLE(ResourceManager)->LoadTexture(L"Start", L"Sprite\\UI\\GameStart.bmp");
	GET_SINGLE(ResourceManager)->LoadTexture(L"TitleBackGround", L"Sprite\\Map\\BackGround.bmp");

	GET_SINGLE(ResourceManager)->CreateSprite(L"TitleBackGround", GET_SINGLE(ResourceManager)->GetTexture(L"TitleBackGround"));
	GET_SINGLE(ResourceManager)->CreateSprite(L"StartOn", GET_SINGLE(ResourceManager)->GetTexture(L"Start"), 0, 0, 140, 60);
	GET_SINGLE(ResourceManager)->CreateSprite(L"StartOff", GET_SINGLE(ResourceManager)->GetTexture(L"Start"), 160, 0, 140, 60);

	{
		BackGround* background = new BackGround();
		background->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"TitleBackGround"));
		UIObjects.push_back(background);
	}

	{
		StartButton* button = new StartButton();
		button->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"StartOff"), BS_Default);
		button->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"StartOn"), BS_Pressed);
		button->SetPos({ 300, 600 });
		button->AddOnClickDelegate(button, &StartButton::OnClicked);
		UIObjects.push_back(button);
	}

	// Window Edit 창 생성
	hEditHandle = CreateWindowEx(
		0,
		L"EDIT", // 윈도우창에는 다양한 클래스가 존재하는데, ms문서를 살펴보자. EDIT은 채팅창 ㅇㅇ
		NULL, // 생성할 윈도우 창 이름
		WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		// WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, // 창 스타일과 편집 컨트롤. ms 문서 살펴보기
		50, // 채팅 창의 X좌표
		400, // 채팅박스의 Y좌표
		450, // 채팅박스의 너비
		20, // 채팅박스의 높이
		_hwnd,
		(HMENU)ID_EDIT,
		NULL,
		NULL);

	{
		for (UI* ui : UIObjects)
		{
			ui->Init();
		}
	}
}

void TitleScene::Update()
{
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::Enter))
	{
		int32 buffersize = 5;

		::GetWindowText(hEditHandle, RecvBuffer, buffersize);

		// Client 이름 적어주기
		wcscpy_s(GET_SINGLE(ClientIocpManager)->ClientName, buffersize, RecvBuffer);
	}

	for (UI* ui : UIObjects)
	{
		ui->Update();
	}
}

void TitleScene::Render(HDC hdc)
{
	if (hEditHandle)
	{
		// ::InvalidateRect(hEditHandle, NULL, FALSE);
		::RedrawWindow(hEditHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	for (UI* ui : UIObjects)
		ui->Render(hdc);
}
