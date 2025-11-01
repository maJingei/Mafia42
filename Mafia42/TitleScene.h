#pragma once
#include "Scene.h"

class UI;

class TitleScene : public Scene
{
	using Super = Scene;
public:
	TitleScene();
	~TitleScene();

	virtual void Init(HWND hwnd) override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;

	virtual void UpdateControl() override;

public:
	HWND hEditHandle = {};
	// UI �����Դϴ�.
	vector<UI*> UIObjects;
	WCHAR RecvBuffer[BUFSIZE] = {};
};

