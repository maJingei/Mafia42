#include "pch.h"
#include "UI.h"
#include "InputManager.h"

UI::UI()
{
}

UI::~UI()
{
}

void UI::Init()
{

}

void UI::Update()
{

}

void UI::Render(HDC hdc)
{

}

RECT UI::GetRect()
{
	RECT rect =
	{
		_pos.x - _size.x / 2,
		_pos.y - _size.y / 2,
		_pos.x + _size.x / 2,
		_pos.y + _size.y / 2
	};

	return rect;
}

bool UI::IsMouseInRect()
{
	RECT rect = GetRect();

	POINT mousePos = GET_SINGLE(InputManager)->GetMousePos();

	return ::PtInRect(&rect, mousePos);
}
