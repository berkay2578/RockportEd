#pragma once
#include "..\..\Memory.h"

namespace GameInternals {
   namespace Data {
      namespace Variables {
         static bool* fadeScreenOn       = (bool*)(0x51CAE4 + 0x400000); // FadeScreen::IsFadeScreenOn
         static bool* resetGameWindow    = (bool*)(0x582C39 + 0x400000);
         static bool* gameWindowInactive = (bool*)(0x582C50 + 0x400000);
         static bool* gameplayActive     = (bool*)(0x4F40C4 + 0x400000);
         static bool* drawHUD            = (bool*)(0x17CAA8 + 0x400000); // really ghetto way, this is actually an assigment in a function

         static bool isFadeScreenOn() {
            return *fadeScreenOn;
         }
         static bool isGameWindowInactive() {
            return *gameWindowInactive;
         }
         static bool isGameplayActive() {
            return *gameplayActive;
         }

         static bool isResetGameWindow() {
            return *resetGameWindow;
         }
         static void setResetGameWindow(const bool& newValue) {
            Memory::openMemoryAccess((DWORD)resetGameWindow, 1);
            *resetGameWindow = newValue;
            Memory::restoreMemoryAccess();
         }

         static bool isDrawHUD() {
            return *drawHUD;
         }
         static void setDrawHUD(const bool& newValue) {
            Memory::openMemoryAccess((DWORD)drawHUD, 1);
            *drawHUD = newValue;
            Memory::restoreMemoryAccess();
         }
      }
   }
}