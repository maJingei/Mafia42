#include "pch.h"
#include "Texture.h"
#include "Sprite.h"
#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{
	Clear();
}

void ResourceManager::Init(HWND hwnd, fs::path resourcePath)
{
	_hwnd = hwnd;
	_resourcePath = resourcePath;
}

Texture* ResourceManager::LoadTexture(const wstring& key, const wstring& path, uint32 transparent)
{
	// �ε��صξ��ٸ� �״�� ã�� texture ����
	if (_textures.find(key) != _textures.end())
	{
		return _textures[key];
	}

	// �ε� �ȵǾ������� ���� ���� �� �ε�
	fs::path fullPath = _resourcePath / path;

	Texture* texture = new Texture();
	texture->LoadBmp(_hwnd, fullPath.c_str());
	texture->SetTransparent(transparent);

	// Map�� ����
	_textures[key] = texture;

	return texture;
}

Sprite* ResourceManager::CreateSprite(const wstring& key, Texture* texture, int32 x, int32 y, int32 cx, int32 cy)
{
	if (_sprites.find(key) != _sprites.end())
		return _sprites[key];

	if (cx == 0)
		cx = texture->GetSize().x;

	if (cy == 0)
		cy = texture->GetSize().y;

	Sprite* sprite = new Sprite(texture, x, y, cx, cy);
	_sprites[key] = sprite;

	return sprite;
}

void ResourceManager::Clear()
{
	for (auto& item : _textures)
	{
		if (item.second)
		{
			delete item.second;
			item.second = nullptr;
		}
	}

	_textures.clear();

	for (auto& item : _sprites)
	{
		if (item.second)
		{
			delete item.second;
			item.second = nullptr;
		}
	}

	_sprites.clear();
}
