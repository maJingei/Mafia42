#include "pch.h"
#include "SceneManager.h"
#include "LobbyScene.h"
#include "GameScene.h"
#include "Scene.h"

void SceneManager::Init()
{

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

	newScene->Init();
}
