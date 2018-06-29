/*
   MIT License
   
   Copyright (c) Berkay Yigit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

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