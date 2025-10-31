#pragma once

class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void Init(HWND hwnd);
	virtual void Update() abstract;
	virtual void Render(HDC hdc) abstract;

protected:
	HWND _hwnd = {};
};

