#pragma once

class Scene;

enum class SceneType
{
	None,
	LobbyScene,
	GameScene,
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	void Init();
	void Update();
	void Render(HDC hdc);

	void Clear();

public:
	void ChangeScene(SceneType sceneType);

private:
	Scene* _scene;
	SceneType _sceneType = SceneType::None;
};

