#include "stdafx.h"
#include "Settings.h"
#include "Memory.h"
#include "Mods.h"
#include "D3D9Hook.h"
#include "DInput8Hook.h"

DWORD WINAPI init(LPVOID) {
	Sleep(5000);
	Settings::Init();
	Mods::Init();
	D3D9Hook::Init();

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hModule);

		Memory::Init();
		DInput8Hook::Init();

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&init, 0, 0, 0);
	}
	return TRUE;
}
