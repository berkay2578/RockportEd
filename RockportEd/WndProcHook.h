#pragma once
#include <WinDef.h>

namespace Hooks {
   namespace WndProc {
      void addExtension(LPVOID extenderAddress);

      DWORD WINAPI Init(LPVOID);
   }
}