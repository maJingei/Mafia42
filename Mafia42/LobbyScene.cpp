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

	// �̹������� Load
	GET_SINGLE(ResourceManager)->LoadTexture(L"LobbyBackGround", L"Sprite\\Map\\LobbyBackGround.bmp");

	GET_SINGLE(ResourceManager)->CreateSprite(L"LobbyBackGround", GET_SINGLE(ResourceManager)->GetTexture(L"LobbyBackGround"));

	{
		BackGround* backGround = new BackGround();
		backGround->SetSprite(GET_SINGLE(ResourceManager)->GetSprite(L"LobbyBackGround"));
		
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

void LobbyScene::Update()
{
	if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::Enter))
	{
		::GetWindowText(hEditHandle, RecvBuffer, BUFSIZE);

		// ä�� �Է��� ���ۿ� ���ڰ� �����־�ߵ�
		if (wcscmp(RecvBuffer, L"") != 0)
		{
			// TODO : ���� ���������� ������ ��Ŷ ���� ����
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
