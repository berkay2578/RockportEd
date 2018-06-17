#pragma once
#include <WinDef.h>
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data::GameFunctions {
      static void Game_ShowPauseMenu() {
         ((void(__cdecl*)())0x6050F0)();
      }
   }
} namespace GameFunctions = GameInternals::Data::GameFunctions;