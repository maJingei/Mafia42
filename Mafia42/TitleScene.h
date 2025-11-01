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
	// UI 모음입니다.
	vector<UI*> UIObjects;
	WCHAR RecvBuffer[BUFSIZE] = {};
};

