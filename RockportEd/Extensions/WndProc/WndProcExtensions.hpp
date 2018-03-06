#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
// Helpers
#include "Helpers\WndProc\WndProcHook.h"
// Extensions
#include "FixAnnoyances.hpp"
#include "WindowedModeImprovements.hpp"

namespace Extensions {
   namespace WndProc {
      static void InitAll() {
         while (!Helpers::WndProcHook::windowHandle) {
            Sleep(100);
         }
         FixAnnoyances::Init();
         if (Settings::settingsType.preferences.WindowedModePreferences.EnableWindowedModeImprovements)
            WindowedModeImprovements::Init();
      }
   }
}