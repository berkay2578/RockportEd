#pragma once
#include "..\Data\Data.h"

namespace GameInternals {
   namespace Config {
      namespace Player {
         static bool isManualTransmissionEnabled() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return false;

            return *(bool*)(*pBase + 0x91);
         }
         static void setManualTransmissionEnabled(const bool& newValue) {
            auto pBase = Data::readPointerBase1();
            if (pBase)
               *(bool*)(*pBase + 0x91) = newValue;
         }

         static bool* getIsShowGameGaugesAsPointer() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return nullptr;

            return (bool*)(*pBase + 0x80);
         }
         static bool isShowGameGauges() {
            auto pBase = getIsShowGameGaugesAsPointer();
            if (pBase)
               return *pBase;

            return false;
         }
         static void setShowGameGauges(const bool& newValue) {
            auto pBase = getIsShowGameGaugesAsPointer();
            if (pBase) {
               *pBase = newValue;
            }
         }
      }
   }
}