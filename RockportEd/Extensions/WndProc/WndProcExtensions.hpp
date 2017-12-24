#pragma once
#include "stdafx.h"
// Extension base
#include "Helpers\WndProc\WndProcHook.h"
// Extensions
#include "FixAnnoyances.hpp"
#include "WindowedModeImprovements.hpp"

namespace Extensions {
   namespace WndProc {
      static void InitAll() {
         while (!Hooks::WndProc::windowHandle) {
            Sleep(100);
         }
         FixAnnoyances::Init();
         WindowedModeImprovements::Init();
      }
   }
}