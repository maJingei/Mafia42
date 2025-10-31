#include "pch.h"
#include "BackGround.h"
#include "Sprite.h"

BackGround::BackGround()
{
}

BackGround::~BackGround()
{
}

void BackGround::Init()
{
}

void BackGround::Update()
{
}

void BackGround::Render(HDC hdc)
{
	Super::Render(hdc);

	if (_sprite == nullptr)
	{
		return;
	}

	Vec2Int size = _sprite->GetSize();

	::BitBlt(hdc,
		(int32)_pos.x - size.x / 2,
		(int32)_pos.y - size.y / 2,
		size.x,
		size.y,
		_sprite->GetDC(),
		_sprite->GetPos().x,
		_sprite->GetPos().y,
		SRCCOPY);
}
