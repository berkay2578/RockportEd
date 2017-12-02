#pragma once
#include "stdafx.h"
#include "WndProcHook.h"

namespace Extensions {
   namespace WndProc {
      namespace FixAnnoyances {
         // TODO: Expose on SettingsType
         static bool disableAltMenu         = true;
         static bool disableStopOnFocusLoss = false;
         static bool disableWindowsMouse    = true;

         static LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            switch (uMsg) {
               case WM_SYSCOMMAND:
                  if ((wParam & 0xFFF0) == SC_KEYMENU && disableAltMenu) {
                     return TRUE;
                  }
                  break;
               case WM_ACTIVATEAPP:
                  if (wParam == FALSE && disableStopOnFocusLoss) {
                     return TRUE;
                  }
                  break;
               case WM_SETCURSOR:
                  if (LOWORD(lParam) == HTCLIENT && disableWindowsMouse) {
                     SetCursor(NULL);
                     return TRUE;
                  }
                  break;
            }
            return FALSE;
         }

         static void Init() {
            Hooks::WndProc::addExtension(&wndProcExtension);
         }
      }
   }
}