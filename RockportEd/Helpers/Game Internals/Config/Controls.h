#pragma once
#include <WinDef.h>

namespace GameInternals {
   namespace Config {
      namespace Controls {
         static BYTE getKeyAccelerate() {
            return *(BYTE*)(0x51F420 + 0x400000);
         }
         static BYTE getKeyBrake() {
            return *(BYTE*)(0x51F454 + 0x400000);
         }
         static BYTE getKeyGearDown() {
            return *(BYTE*)(0x51F58C + 0x400000);
         }
         static BYTE getKeyGearUp() {
            return *(BYTE*)(0x51F5C0 + 0x400000);
         }
      }
   }
}