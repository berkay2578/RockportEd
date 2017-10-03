#pragma once
#include "stdafx.h"

namespace D3D9Hook {
   enum class HookOptions {
      CustomCamera,
      Replay,
      NewHUD,
      GameSpeed
   };

   namespace Settings {
      extern bool blockKeyboard;
      extern bool blockMouse;
   }

   DWORD WINAPI Init(LPVOID);
}