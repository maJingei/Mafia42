#pragma once

// Message 구조체
struct FMessage
{
	WCHAR Message[BUFSIZE];
	RECT rect;
	COLORREF color = RGB(255, 0, 255);
};

class MessageManager
{
public:
	DECLARE_SINGLE(MessageManager);
	~MessageManager();

public:
	void Init(HWND hwnd);
	void Update();
	void Render(HDC hdc);


	void PushMessage(const char* message);
	void PushTimeMessage(const char* timemessage);

public:
	HWND _hwnd;
	// 덱으로 관리하면서 가장 오래된 메세지는 삭제해버리기
	Deque<FMessage> MessageDeque;

	// 시간을 그냥 String으로 받아서 
	FMessage TimeMessage;
};

