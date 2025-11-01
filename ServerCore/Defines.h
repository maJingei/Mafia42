#pragma once

#define DECLARE_SINGLE(classname)				\
private:										\
	classname() {}								\
public:											\
	static classname* GetInstance()				\
	{											\
		static classname s_instance;			\
												\
		return &s_instance;						\
	}											

#define GET_SINGLE(classname)	classname::GetInstance()

#define BUFSIZE 1024

#define WINSIZEX 600
#define WINSIZEY 800

#define ID_EDIT 100

#define WM_USER_CHAT_RECV (WM_USER + 1)
#define WM_USER_CHAT_CONNECT (WM_USER + 2)
#define WM_USER_GAME_START (WM_USER + 3)
#define WM_USER_TIME (WM_USER + 4)