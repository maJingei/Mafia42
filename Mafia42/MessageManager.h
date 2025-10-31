#pragma once

// Message 구조체
struct FMessage
{
	WCHAR Message[BUFSIZE];
	RECT rect;
};

class MessageManager
{
public:
	DECLARE_SINGLE(MessageManager);
	~MessageManager();

public:
	void Init(HWND hwnd);
	void Update();
	void Render();


	void PushMessage(const char* message);

public:
	HWND _hwnd;
	// 덱으로 관리하면서 가장 오래된 메세지는 삭제해버리기
	Deque<FMessage> MessageDeque;
};

