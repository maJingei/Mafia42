#include "pch.h"
#include "ResourceManager.h"
#include "MessageManager.h"
#include "BackGround.h"
#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
}

void GameScene::Init(HWND hwnd)
{
	Super::Init(hwnd);

	GET_SINGLE(ResourceManager)->LoadTexture(L"GameBackGround", L"Sprite\\Map\\GameBackGround.bmp");

	GET_SINGLE(ResourceManager)->CreateSprite(L"GameBackGround", GET_SINGLE(ResourceManager)->GetTexture(L"GameBackGround"));

	{
		BackGround* backGround = new BackGround();
		backGround->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"GameBackGround"));
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

void GameScene::Update()
{
	ChatUpdate();

	for (UI* item : UIObjects)
	{
		item->Update();
	}
}

void GameScene::Render(HDC hdc)
{
	for (UI* item : UIObjects)
	{
		item->Render(hdc);
	}

	GET_SINGLE(MessageManager)->Render(hdc);

	if (hEditHandle)
	{
		// ::InvalidateRect(hEditHandle, NULL, FALSE);
		::RedrawWindow(hEditHandle, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}