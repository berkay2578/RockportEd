#include "stdafx.h"
// Helpers
#include "Game Internals\GameInternals.h"
#include "Settings.h"
#include "WndProcHook.h"
// Extensions
#include "InGameMenu.h"
#include "_LoadInGameMenuItems.hpp"
#include "DI8Extension.h"
#include "WndProcExtensions.hpp"

DWORD WINAPI Init(LPVOID) {
   // Helpers
   Memory::Init();
   Settings::loadSettings();
   Hooks::WndProc::Init();

   // Extensions
   Extensions::WndProc::InitAll();
   Extensions::InGameMenu::Init();
   Extensions::DI8::Init();

   // InGameMenu Items
   Extensions::InGameMenu::loadItemsToInGameMenu();
   return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
   if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
      DisableThreadLibraryCalls(hModule);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, NULL);
   }
   return TRUE;
}