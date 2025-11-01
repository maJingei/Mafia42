#pragma once

enum EGamePhase
{
	DAY,
	VOTENIGHT,
	NIGHT
};

/*
* �������� ����˴ϴ�.
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

	// Time ���� ������
private:
	uint64 _frequency = 0;
	uint64 _prevCount = 0;
	float _deltaTime = 0.f;

	float time = 300;

private:
	// GameThread�� �� �̺�Ʈ�� �ñ׳λ��°� ���� �� ����鼭 ������ ����� �� �Դϴ�.
	HANDLE GameEventHandle;
};

