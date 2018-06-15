#pragma once
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data {
      namespace Variables {
         //broken, doesn't work if the player doesn't use the first save file slot... static bool* gameplayActive     = (bool*)(0x4F40C4 + 0x400000);
         //broken, doesn't work if the player doesn't use the first save file slot... static bool* gameplayAccessed   = (bool*)(0x51CD38 + 0x400000);

         template <typename T>
         inline void setVariable(LPVOID variable, const T& newValue) {
            Memory::openMemoryAccess((DWORD)variable, sizeof(T));
            *(T*)variable = newValue;
            Memory::restoreMemoryAccess();
         }

         static bool*    fadeScreenOn       = (bool*)(0x51CAE4 + 0x400000); // FadeScreen::IsFadeScreenOn
         static bool*    resetGameWindow    = (bool*)(0x582C39 + 0x400000);
         static bool*    gameWindowInactive = (bool*)(0x582C50 + 0x400000);
         static bool*    drawHUD            = (bool*)(0x17CAA8 + 0x400000); // really ghetto way, this is actually an assigment in a function
         static int16_t* nosFOVWidening     = (int16_t*)(0x51112C + 0x400000);

         static bool isFadeScreenOn() {
            return *fadeScreenOn;
         }
         static bool isGameWindowInactive() {
            return *gameWindowInactive;
         }
         static bool isResetGameWindow() {
            return *resetGameWindow;
         }
         static bool isDrawHUD() {
            return *drawHUD;
         }
         static int16_t getNosFOVWidening() {
            return *nosFOVWidening;
         }
      }
   }
} namespace InternalVariables = GameInternals::Data::Variables;