#pragma once
#include <WinDef.h>

namespace Hooks {
   namespace WndProc {
      extern HWND windowHandle;
      void addExtension(LPVOID extenderAddress);

      void Init();
   }
}