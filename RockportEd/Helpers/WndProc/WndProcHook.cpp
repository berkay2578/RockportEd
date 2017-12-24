#include "stdafx.h"
#include "WndProcHook.h"
#include MIRRORHOOK_DEFINITIONS_PATH

namespace Hooks {
   namespace WndProc {
      HWND                 windowHandle = nullptr;
      WNDPROC              origWndProc  = nullptr;
      std::vector<WNDPROC> extensions   = std::vector<WNDPROC>();

      void addExtension(LPVOID extensionAddress) {
         extensions.push_back(reinterpret_cast<WNDPROC>(extensionAddress));
      }

      UINT nRetTRUE = 0;
      LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
         if (!extensions.empty()) {
            for (WNDPROC wndProcExtender : extensions) {
               if (wndProcExtender(hWnd, uMsg, wParam, lParam))
                  nRetTRUE++;
            }
            if (nRetTRUE) {
               nRetTRUE = 0;
               return TRUE;
            }
         }

         return CallWindowProcA(origWndProc, hWnd, uMsg, wParam, lParam);
      }

      void Init() {
         while (!windowHandle) {
            windowHandle = MirrorHook::D3D9::GetWindowHandle();
            Sleep(100);
         }
         origWndProc = (WNDPROC)SetWindowLongPtrA(windowHandle, GWL_WNDPROC, (LONG_PTR)&hkWndProc);
      }
   }
}