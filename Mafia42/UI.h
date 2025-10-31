#pragma once
class UI
{
public:
	UI();
	virtual ~UI();

	virtual void Init();
	virtual void Update();
	virtual void Render(HDC hdc);

	void SetPos(Vec2 pos) { _pos = pos; }
	Vec2 GetPos() { return _pos; }

	RECT GetRect();
	bool IsMouseInRect();

protected:
	Vec2 _pos = { 300, 400 };
	Vec2Int _size = { 150, 150 };
};

