#include "stdafx.h"
#include "Memory.h"
#include "Mods.h"
#include "D3D9Hook.h"
#include "DInput8Hook.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
   if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
      DisableThreadLibraryCalls(hModule);

      //Settings::Init(); just rewrite
      Memory::Init();

      DInput8Hook::Init(); // generic hook

      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Mods::Init, 0, 0, 0);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&D3D9Hook::Init, 0, 0, 0); // nfsmw specific hook
   }
   return TRUE;
}
