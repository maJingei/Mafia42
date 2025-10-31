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
	// �̹��� Load
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

	// Window Edit â ����
	hEditHandle = CreateWindowEx(
		0,
		L"EDIT", // ������â���� �پ��� Ŭ������ �����ϴµ�, ms������ ���캸��. EDIT�� ä��â ����
		NULL, // ������ ������ â �̸�
		WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		// WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL, // â ��Ÿ�ϰ� ���� ��Ʈ��. ms ���� ���캸��
		50, // ä�� â�� X��ǥ
		400, // ä�ùڽ��� Y��ǥ
		450, // ä�ùڽ��� �ʺ�
		20, // ä�ùڽ��� ����
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

		// Client �̸� �����ֱ�
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
