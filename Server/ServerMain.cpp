#include "pch.h"
#include "CoreGlobal.h"
#include "IocpManager.h"
#include "ThreadManager.h"


int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	IocpManager* iocpManager = new IocpManager();

	iocpManager->Begin();


	// iocpManager->WorkerThread();

	::WSACleanup();
}
