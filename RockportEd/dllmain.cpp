#include "stdafx.h"
#define CEREAL_XML_STRING_VALUE "RockportEd"

// Helpers
#include "Memory.h"
#include "Settings.h"
#include "WndProcHook.h"
// Extensions
#include "D3D9Extension.h"
#include "DI8Extension.h"
#include "WndProcExtensions.hpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
   if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
      DisableThreadLibraryCalls(hModule);

      // Helpers
      Memory::Init();
      Settings::loadSettings();
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Hooks::WndProc::Init, NULL, 0, NULL);

      // Extensions
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Extensions::D3D9::Init, NULL, 0, NULL);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Extensions::DI8::Init, NULL, 0, NULL);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Extensions::WndProc::InitAll, NULL, 0, NULL);
   }
   return TRUE;
}