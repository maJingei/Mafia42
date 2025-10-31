#pragma once
#include "UI.h"

class Sprite;

class BackGround : public UI
{
	using Super = UI;
public:
	BackGround();
	virtual ~BackGround();

	virtual void Init() override;
	virtual void Update() override;
	virtual void Render(HDC hdc) override;

	void SetSprite(Sprite* sprite) { _sprite = sprite; }

protected:
	Sprite* _sprite = nullptr;
};

