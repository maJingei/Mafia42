#include "pch.h"
#include "IocpManager.h"
#include "GameManager.h"

GameManager::~GameManager()
{

}

void GameManager::GameThread()
{
	// Event가 Signaled가 될 때까지 Thread가 대기상태로 기다립니다.
	::WaitForSingleObject(GameEventHandle, INFINITE);

	// 시작 @@!!
	cout << "GameStart ! " << endl;

	// 역할 분배
	ShuffleRole();

	float deltaTime = 0;
	while (true)
	{
		TimeUpdate();
		deltaTime += GetDeltaTime();
		if (deltaTime >= 1.f)
		{
			time -= 1;
			deltaTime = 0.f;

			TimeBroadcast(time);
		}

		Update();
	}
}

void GameManager::ShuffleRole()
{
	// TODO : 역할 분배해서 Client들에게 Send
	GET_SINGLE(IocpManager)->NetworkShuffleRole();
}

void GameManager::TimeBroadcast(int32 time)
{
	std::string TimeText = std::to_string(time);
	GET_SINGLE(IocpManager)->TimeBroadcast((BYTE*)TimeText.c_str(), TimeText.size());
}


void GameManager::Init()
{
	GameEventHandle = ::CreateEvent(NULL, // 보안관련
		FALSE, // 수동 재설정 이벤트 객체 생성
		FALSE, // 초기 상태가 signaled
		NULL); // 이벤트 객체 이름

	// CPU 클럭과 주파수 땡겨오기
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU 클럭
}

void GameManager::Update()
{
}

void GameManager::Clear()
{
}

void GameManager::TimeUpdate()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;
}

void GameManager::SetGameEvent()
{
	// 시그널 상태로 !
	::SetEvent(GameEventHandle);
}
