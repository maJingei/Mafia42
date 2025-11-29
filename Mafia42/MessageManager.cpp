#include "pch.h"
#include "MessageManager.h"

MessageManager::~MessageManager()
{
}

void MessageManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
}

void MessageManager::Update()
{
	if (MessageDeque.empty() == false)
	{
		for (auto& msg : MessageDeque)
		{
			msg.rect.top -= 40;
			msg.rect.bottom -= 40;

			if (msg.rect.top <= 200)
			{
				MessageDeque.pop_front();
			}
		}
	}
}

void MessageManager::Render(HDC hdc)
{
	if (MessageDeque.empty() == false)
	{
		for (auto& msg : MessageDeque)
		{
			::SetTextColor(hdc, msg.color);
			::SetBkMode(hdc, TRANSPARENT);

			::DrawText(hdc, msg.Message, -1, &msg.rect, DT_VCENTER | DT_WORDBREAK);
		}
	}
	
	// 시간은 따로
	::SetTextColor(hdc, TimeMessage.color);
	::SetBkMode(hdc, TRANSPARENT);
	::DrawText(hdc, TimeMessage.Message, -1, &TimeMessage.rect, DT_VCENTER | DT_WORDBREAK);
}

void MessageManager::PushMessage(const char* message)
{
	FMessage msg;
	msg.rect.top = 650;
	msg.rect.bottom = 700;
	msg.rect.left = 20;
	msg.rect.right = 480;
	msg.color = RGB(255, 255, 255);

	WCHAR* buffer;
	int32 charsize = MultiByteToWideChar(CP_ACP, 0, message, -1, NULL, NULL);
	buffer = new WCHAR[charsize];
	MultiByteToWideChar(CP_ACP, 0, message, strlen(message) + 1, buffer, charsize);
	wcscpy_s(msg.Message, charsize, buffer);

	delete buffer;
	buffer = nullptr;

	MessageDeque.push_back(msg);
}

void MessageManager::PushTimeMessage(const char* timemessage)
{
	FMessage msg;
	msg.rect.top = 100;
	msg.rect.bottom = 200;
	msg.rect.left = 250;
	msg.rect.right = 450;
	msg.color = RGB(255,0,0);

	WCHAR* buffer;
	int32 charsize = MultiByteToWideChar(CP_ACP, 0, timemessage, -1, NULL, NULL);
	buffer = new WCHAR[charsize];
	MultiByteToWideChar(CP_ACP, 0, timemessage, strlen(timemessage) + 1, buffer, charsize);
	wcscpy_s(msg.Message, charsize, buffer);

	delete buffer;
	buffer = nullptr;

	TimeMessage = msg;
}
