#pragma once
#include "stdafx.h"
#include <map>

namespace Mods {
	extern int* activeCamera;
	extern std::map<int, std::map<char*, float*>> cameraData;

	DWORD WINAPI Init(LPVOID);
}