#include "stdafx.h"
#include "WndProcHook.h"

#include <vector>
using std::vector;

#include MIRRORHOOK_DEFINITIONS_PATH

namespace Hooks {
   namespace WndProc {
      HWND            windowHandle = nullptr;
      WNDPROC         origWndProc  = nullptr;
      vector<WNDPROC> extensions   = vector<WNDPROC>();

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

      DWORD WINAPI Init(LPVOID) {
         HMODULE hMirrorHook = nullptr;
         while (!hMirrorHook) {
            hMirrorHook = GetModuleHandleA("MirrorHook.asi");
            Sleep(100);
         }
         while (!MirrorHook::D3D9::IsReady()) {
            Sleep(100);
         }
         windowHandle = MirrorHook::D3D9::GetWindowHandle();
         origWndProc = (WNDPROC)SetWindowLongPtrA(windowHandle, GWL_WNDPROC, (LONG_PTR)&hkWndProc);

         return TRUE;
      }
   }
}