#pragma once
#include <WinDef.h>

namespace Hooks {
   namespace WndProc {
      extern HWND windowHandle;
      void addExtension(LPVOID extenderAddress);

      DWORD WINAPI Init(LPVOID);
   }
}