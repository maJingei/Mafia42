#include "pch.h"
#include "SceneManager.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "TitleScene.h"

void SceneManager::Init(HWND hwnd)
{
	_hwnd = hwnd;
}

void SceneManager::Update()
{
	if (_scene)
		_scene->Update();
}

void SceneManager::Render(HDC hdc)
{
	if (_scene)
		_scene->Render(hdc);
}

void SceneManager::Clear()
{
	if (_scene)
	{
		delete _scene;
		_scene = nullptr;
	}
}



void SceneManager::ChangeScene(SceneType sceneType)
{
	if (_sceneType == sceneType)
		return;

	Scene* newScene = nullptr;

	switch (sceneType)
	{
	case SceneType::TitleScene:
		newScene = new TitleScene();
		break;
		case SceneType::LobbyScene:
		newScene = new LobbyScene();
		break;
	case SceneType::GameScene:
		newScene = new GameScene();
		break;
	}

	if (_scene)
	{
		delete _scene;
		_scene = nullptr;
	}

	_scene = newScene;
	_sceneType = sceneType;

	// Game의 Init에서 SceneManager가 Init을 진행하니 ChangeScene에서는 Init을 할 필요가 .. ? 
	// 근데 그러면 Scene이 Change될 때 Init이 필요함. 
	newScene->Init(_hwnd);
}
