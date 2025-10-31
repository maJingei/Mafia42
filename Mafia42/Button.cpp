#include "pch.h"
#include "Button.h"
#include "InputManager.h"
#include "TimeManager.h"
#include "Sprite.h"

Button::Button()
{
}

Button::~Button()
{
}

void Button::Init()
{
	Super::Init();

	SetButtonState(BS_Default);
}

void Button::Update()
{
	POINT mousePos = GET_SINGLE(InputManager)->GetMousePos();
	float deltaTime = GET_SINGLE(TimeManager)->GetDeltaTime();

	// 0.2ms �̻� �Ѿ�� default ���·� �Ѿ Ŭ�� �����ٴ� ��
	if (_state == BS_Clicked)
	{
		_sumTime += deltaTime;
		if (_sumTime >= 0.2f)
		{
			_sumTime = 0.f;
			SetButtonState(BS_Default);
		}
	}

	// ���콺�� ��ư �ȿ� ������
	if (IsMouseInRect())
	{
		// OnPressed ����
		if (GET_SINGLE(InputManager)->GetButton(KeyType::LeftMouse))
		{
			SetButtonState(BS_Pressed);
		}
		else
		{
			// Pressed ���¿����� Ŭ����
			if (_state == BS_Pressed)
			{
				SetButtonState(BS_Clicked);
				if (_onClick)
					_onClick();
			}
		}
	}
	else
	{
		SetButtonState(BS_Default);
	}
}

void Button::Render(HDC hdc)
{
	if (_currentSprite)
	{
		::TransparentBlt(hdc,
			(int32)_pos.x - _size.x / 2,
			(int32)_pos.y - _size.y / 2,
			_size.x,
			_size.y,
			_currentSprite->GetDC(),
			_currentSprite->GetPos().x,
			_currentSprite->GetPos().y,
			_currentSprite->GetSize().x,
			_currentSprite->GetSize().y,
			_currentSprite->GetTransparent());
	}
	else
	{
		RECT rect = { 200, 200 };
		// TODO : Render ����
		::DrawText(hdc, L"Faild", 5, &rect, DT_CENTER);
	}
}

void Button::SetButtonState(ButtonState state)
{
	_state = state;

	if (_sprites[state])
		SetCurrentSprite(_sprites[state]);
}
