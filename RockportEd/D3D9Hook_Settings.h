#pragma once
#include "stdafx.h"

namespace D3D9HookSettings {
   extern bool isImguiInitialized;

   extern bool blockKeyboard;
   extern bool blockMouse;

   namespace Options {
      extern bool isMainWindowVisible;

      extern bool opt_CustomCamera;
      extern bool opt_ReplayMenu;
   }
}