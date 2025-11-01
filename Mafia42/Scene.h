#pragma once

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Init(HWND hwnd);
	virtual void Update() abstract;
	virtual void Render(HDC hdc) abstract;

	virtual void ChatUpdate();

	virtual void UpdateControl() {}

protected:
	HWND _hwnd = {};

	// Windowâ Editâ�� ���� Handle
	HWND hEditHandle;

	// ä�� �޾��� recvBuffer
	WCHAR RecvBuffer[BUFSIZE] = {};
};

