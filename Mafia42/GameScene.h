#pragma once
#include "Scene.h"

class UI;

class GameScene : public Scene
{
	using Super = Scene;
public:
	GameScene();
	virtual ~GameScene() override;

	virtual void Init(HWND hwnd) override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;


private:
	vector<UI*> UIObjects;
};

