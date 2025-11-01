#pragma once

// Message ����ü
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
	// ������ �����ϸ鼭 ���� ������ �޼����� �����ع�����
	Deque<FMessage> MessageDeque;

	// �ð��� �׳� String���� �޾Ƽ� 
	FMessage TimeMessage;
};

