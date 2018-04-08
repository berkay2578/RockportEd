#pragma once
#include "stdafx.h"
#include <Helpers\imgui\imgui.h>

namespace Extensions {
   namespace InGameMenu {
      class _BaseInGameMenuItem {
      public:
         bool hasLoadedData            = false;
         const virtual void loadData() = 0;

         const virtual void onFrame() = 0;

         const virtual bool displayMenuItem(const ImVec2& buttonSize = ImVec2()) = 0;
         const virtual bool displayMenu() = 0;
      };
   }
}