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

	// Window Edit â ����
	hEditHandle = CreateWindowEx(
		0,
		L"EDIT", // ������â���� �پ��� Ŭ������ �����ϴµ�, ms������ ���캸��. EDIT�� ä��â ����
		NULL, // ������ ������ â �̸�
		WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		// WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, // â ��Ÿ�ϰ� ���� ��Ʈ��. ms ���� ���캸��
		50, // ä�� â�� X��ǥ
		700, // ä�ùڽ��� Y��ǥ
		450, // ä�ùڽ��� �ʺ�
		20, // ä�ùڽ��� ����
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