#pragma once
#include <WinDef.h>

namespace Helpers {
   namespace WndProcHook {
      extern HWND windowHandle;
      void addExtension(LPVOID extenderAddress);

      void Init();
   }
}