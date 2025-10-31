#pragma once
#include "Scene.h"

class UI;

class LobbyScene : public Scene
{
	using Super = Scene;
public:
	LobbyScene();
	virtual ~LobbyScene() override;

	virtual void Init(HWND hwnd) override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;

private:
	HWND hEditHandle;

	WCHAR RecvBuffer[BUFSIZE] = {};

	vector<UI*> UIObjects;
};

