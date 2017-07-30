#include "stdafx.h"
#include "Settings.h"
#include "Memory.h"
#include "D3D9Hook.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		Settings::Init();
		Memory::Init();
		D3D9Hook::Init();
	}
	return TRUE;
}
