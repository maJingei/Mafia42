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

	// Window창 Edit창을 위한 Handle
	HWND hEditHandle;

	// 채팅 받아줄 recvBuffer
	WCHAR RecvBuffer[BUFSIZE] = {};
};

