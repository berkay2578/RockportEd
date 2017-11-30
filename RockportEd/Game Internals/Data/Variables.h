#pragma once

namespace GameInternals {
   namespace Data {
      namespace Variables {
         static bool isGameWindowInactive() {
            return *(bool*)(0x582C50 + 0x400000);
         }
         static bool isGameplayActive() {
            return *(bool*)(0x4F40C4 + 0x400000);
         }
         static bool isGameInitialized() {
            return *(bool*)(0x51CD38 + 0x400000);
         }
         static bool isFadeScreenOn() { // FadeScreen::IsFadeScreenOn
            return *(bool*)(0x51CAE4 + 0x400000);
         }
      }
   }
}