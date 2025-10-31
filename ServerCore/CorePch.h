#pragma once
#include "CoreGlobal.h"
using namespace std;

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "mswsock.lib")

// STL
#include <functional>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <future>
#include <iostream>
#include <string>

// Utils
#include "Types.h"
#include "PacketTypes.h"
#include "PacketSession.h"
#include "NetworkSession.h"

// Thread
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>

// FileSystem : C++ 20 설정필요
#include <format>
#include <filesystem>
namespace fs = std::filesystem;

#pragma comment(lib, "msimg32.lib")