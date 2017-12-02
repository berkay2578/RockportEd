#pragma once
#include "stdafx.h"
#include "WndProcHook.h"
#include "FixAnnoyances.hpp"
#include "WindowedModeImprovements.hpp"

namespace Extensions {
   namespace WndProc {
      static DWORD WINAPI InitAll(LPVOID) {
         while (!Hooks::WndProc::windowHandle) {
            Sleep(100);
         }
         FixAnnoyances::Init();
         WindowedModeImprovements::Init();

         return TRUE;
      }
   }
}