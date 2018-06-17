#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
// Helpers
#include "Helpers\WndProc\WndProcHook.h"

namespace Extensions {
   namespace WndProc {
      namespace FixAnnoyances {
         static LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            switch (uMsg) {
               case WM_SYSCOMMAND:
                  if ((wParam & 0xFFF0) == SC_KEYMENU) {
                     return Settings::settingsType.preferences.WindowedModePreferences.DisableAltMenu;
                  }
                  break;
               case WM_ACTIVATEAPP:
                  if (wParam == FALSE) {
                     bool shouldPause = Settings::settingsType.preferences.PauseGameplayOnFocusLoss;
                     if (shouldPause)
                        GameFunctions::Game_ShowPauseMenu();

                     return !shouldPause;
                  }
                  break;
               case WM_SETCURSOR:
                  if (LOWORD(lParam) == HTCLIENT && Settings::settingsType.preferences.WindowedModePreferences.DisableWindowsMouse) {
                     SetCursor(NULL);
                     return TRUE;
                  }
                  break;
            }
            return FALSE;
         }

         static void Init() {
            Helpers::WndProcHook::addExtension(&wndProcExtension);
         }
      }
   }
}