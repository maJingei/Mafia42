#include "pch.h"
#include "Game.h"
#include "TitleScene.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "ThreadManager.h"
#include "ClientIocpManager.h"
#include "ResourceManager.h"

Game::Game()
{
}

Game::~Game()
{
	GET_SINGLE(SceneManager)->Clear();
	GET_SINGLE(ResourceManager)->Clear();
	

	_CrtDumpMemoryLeaks();
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_hdc = ::GetDC(hwnd);

	::GetClientRect(hwnd, &_rect);

	hdcBack = ::CreateCompatibleDC(_hdc); // hdc�� ȣȯ�Ǵ� DC�� ����
	_bmpBack = ::CreateCompatibleBitmap(_hdc, _rect.right, _rect.bottom); // hdc�� ȣȯ�Ǵ� ��Ʈ�� ����
	HBITMAP prev = (HBITMAP)::SelectObject(hdcBack, _bmpBack); // DC�� BMP�� ����
	::DeleteObject(prev);

	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		return; // WSA ���̺귯�� �ʱ�ȭ
	}

	if (GET_SINGLE(ClientIocpManager)->Init(_hwnd) == false)
	{
		// TODO : ��Ʈ��ũ ���� ����
	}

	GET_SINGLE(TimeManager)->Init();
	GET_SINGLE(InputManager)->Init(hwnd);
	GET_SINGLE(SceneManager)->Init(hwnd);
	GET_SINGLE(ResourceManager)->Init(hwnd, fs::path(L"D:\\Mafia42\\Resources"));

	GET_SINGLE(SceneManager)->ChangeScene(SceneType::TitleScene);

	GThreadManager->Launch([]()
		{
			GET_SINGLE(ClientIocpManager)->WorkerThread();
		});
}

void Game::Update()
{
	GET_SINGLE(TimeManager)->Update();
	GET_SINGLE(InputManager)->Update();
	GET_SINGLE(SceneManager)->Update();
}

void Game::Render()
{
	GET_SINGLE(SceneManager)->Render(hdcBack);

	// Double Buffering
	::BitBlt(_hdc, 0, 0, _rect.right, _rect.bottom, hdcBack, 0, 0, SRCCOPY); // ��Ʈ �� : ��� ����
	::PatBlt(hdcBack, 0, 0, _rect.right, _rect.bottom, WHITENESS); // backBuffer �о��ֱ�

	GET_SINGLE(SceneManager)->UpdateControl();
}
