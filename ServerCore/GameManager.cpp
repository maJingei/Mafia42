#include "pch.h"
#include "IocpManager.h"
#include "GameManager.h"

GameManager::~GameManager()
{

}

void GameManager::GameThread()
{
	// Event�� Signaled�� �� ������ Thread�� �����·� ��ٸ��ϴ�.
	::WaitForSingleObject(GameEventHandle, INFINITE);

	// ���� @@!!
	cout << "GameStart ! " << endl;

	// ���� �й�
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
	// TODO : ���� �й��ؼ� Client�鿡�� Send
	GET_SINGLE(IocpManager)->NetworkShuffleRole();
}

void GameManager::TimeBroadcast(int32 time)
{
	std::string TimeText = std::to_string(time);
	GET_SINGLE(IocpManager)->TimeBroadcast((BYTE*)TimeText.c_str(), TimeText.size());
}


void GameManager::Init()
{
	GameEventHandle = ::CreateEvent(NULL, // ���Ȱ���
		FALSE, // ���� �缳�� �̺�Ʈ ��ü ����
		FALSE, // �ʱ� ���°� signaled
		NULL); // �̺�Ʈ ��ü �̸�

	// CPU Ŭ���� ���ļ� ���ܿ���
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU Ŭ��
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
	// �ñ׳� ���·� !
	::SetEvent(GameEventHandle);
}
