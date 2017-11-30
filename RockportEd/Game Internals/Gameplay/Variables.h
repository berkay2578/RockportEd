#pragma once

namespace GameInternals {
   namespace Gameplay {
      namespace Variables {
         static float* gameplaySpeed   = (float*)(0x501B1C + 0x400000);
         static int*   simulationMode  =   (int*)(0x525E90 + 0x400000);
         static float* someRpmConstant = (float*)(0x497674 + 0x400000);

         static float getSomeRpmConstant() {
            return *someRpmConstant;
         }
         static void setSomeRpmConstant(const float& newValue) {
            Memory::openMemoryAccess((DWORD)someRpmConstant, 4);
            *someRpmConstant = newValue;
            Memory::restoreMemoryAccess();
         }

         static float* getTrafficRefreshIntervalAsPointer() {
            auto pBase = (float*)Memory::readPointer(0x50DCC0, 3, 0x58, 0x3DC, 0x134);
            if (!pBase)
               return nullptr;

            return pBase;
         }
         static float getTrafficRefreshInterval() {
            auto p = getTrafficRefreshIntervalAsPointer();
            return p ? *p : -1.0f;
         }
         static void setTrafficRefreshInterval(const float& newValue) {
            auto p = getTrafficRefreshIntervalAsPointer();
            if (p) {
               *p = newValue;
            }
         }
      }
   }
}