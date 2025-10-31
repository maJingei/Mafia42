#pragma once

// Message ����ü
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
	// ������ �����ϸ鼭 ���� ������ �޼����� �����ع�����
	Deque<FMessage> MessageDeque;
};

