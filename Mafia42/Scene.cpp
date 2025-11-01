#include "pch.h"
#include "InputManager.h"
#include "ClientIocpManager.h"
#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Init(HWND hwnd)
{
	_hwnd = hwnd;
}

void Scene::ChatUpdate()
{
	if (GET_SINGLE(InputManager)->GetButtonDown(KeyType::Enter))
	{
		::GetWindowText(hEditHandle, RecvBuffer, BUFSIZE);

		// ä�� �Է��� ���ۿ� ���ڰ� �����־�ߵ�
		if (wcscmp(RecvBuffer, L"") != 0)
		{
			// WCHAR to char
			int32 StrSize = WideCharToMultiByte(CP_ACP, 0, RecvBuffer, -1, NULL, 0, NULL, NULL);
			char* charSendBuffer = new char[StrSize];
			WideCharToMultiByte(CP_ACP, 0, RecvBuffer, -1, charSendBuffer, StrSize, 0, 0);

			// Packet ���� ����
			PacketSession* TempSession = GET_SINGLE(ClientIocpManager)->GetSession();
			if (TempSession && TempSession->CreatePacket(EPACKET_TYPE::CHAT, (BYTE*)charSendBuffer, StrSize) != false)
			{
				TempSession->Send(GET_SINGLE(ClientIocpManager)->GetSession()->GetPacket());
			}

			SetWindowText(hEditHandle, TEXT(""));
		}
	}
}
