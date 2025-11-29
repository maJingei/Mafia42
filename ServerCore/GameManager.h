#pragma once

enum EGamePhase
{
	DAY,
	VOTENIGHT,
	NIGHT
};

/*
* 서버에서 진행됩니다.
*/
class GameManager
{
public:
	DECLARE_SINGLE(GameManager)
	~GameManager();

	void GameThread();
	void ShuffleRole();
	void TimeBroadcast(int32 time);

	virtual void Init();
	virtual void Update();
	virtual void Clear();
	void TimeUpdate();

	void SetGameEvent();
	
	float GetDeltaTime() { return _deltaTime; }

	// Time 관련 변수들
private:
	uint64 _frequency = 0;
	uint64 _prevCount = 0;
	float _deltaTime = 0.f;

	float time = 300;

private:
	// GameThread는 이 이벤트가 시그널상태가 됐을 때 깨어나면서 로직이 실행될 것 입니다.
	HANDLE GameEventHandle;
};

