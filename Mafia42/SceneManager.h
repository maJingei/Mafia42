#pragma once

class Scene;

enum class SceneType
{
	None,
	TitleScene,
	LobbyScene,
	GameScene,
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	void Init(HWND hwnd);
	void Update();
	void Render(HDC hdc);

	void Clear();

	void UpdateControl();

public:
	void ChangeScene(SceneType sceneType);

private:
	Scene* _scene;
	SceneType _sceneType = SceneType::None;
	HWND _hwnd;
};

