#include "pch.h"
#include "Game.h"
#include "ClientIocpManager.h"

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(HWND hwnd)
{
	_hwnd = hwnd;
	_hdc = ::GetDC(hwnd);
}

void Game::Update()
{
	
}

void Game::Render()
{
}
