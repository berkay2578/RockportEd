#pragma once
#include "Items\_BaseInGameMenuItem.hpp"

namespace Extensions {
   namespace InGameMenu {
      extern std::vector<_BaseInGameMenuItem*> items;
      extern RECT clientRect;
      void Init();
   }
}