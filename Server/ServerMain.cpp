#include "pch.h"
#include "CoreGlobal.h"
#include "IocpManager.h"
#include "GameManager.h"
#include "ThreadManager.h"


int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	GET_SINGLE(GameManager)->Init();
	GET_SINGLE(IocpManager)->Begin();



	// iocpManager->WorkerThread();

	::WSACleanup();
}
