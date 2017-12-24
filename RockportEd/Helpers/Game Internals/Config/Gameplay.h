#pragma once
#include "..\Data\Data.h"
#include "..\Gameplay\Gameplay.h"

namespace GameInternals {
   namespace Config {
      namespace Gameplay {
         static SpeedUnit getSpeedUnitType() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return SpeedUnit::MPS;

            return *(SpeedUnit*)(*pBase + 0x3B);
         }
         static void setSpeedUnitType(const SpeedUnit& newValue) {
            auto pBase = Data::readPointerBase1();
            if (pBase)
               *(SpeedUnit*)(*pBase + 0x3B) = newValue;
         }
      }
   }
}