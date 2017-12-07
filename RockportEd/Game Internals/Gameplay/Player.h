#pragma once
#include <WinDef.h>
#include "..\Data\Data.h"

namespace GameInternals {
   struct CarPowerData {
      /*         struct align            */ BYTE unk[11 * 0x4];
      float alignedCurrentRPM;           // 0x2C
      /*         struct align            */ BYTE unk2[1 * 0x4];
      float minimumRPM;                  // 0x34
      float maximumPossibleRPM;          // 0x38
      float maximumRPM;                  // 0x3C
      float currentPedalInputPercentage; // 0x40
      /*         struct align            */ BYTE unk3[19 * 0x4];
      int   currentGear;                 // 0x8C
      int   lastGear;                    // 0x90
   };

   enum class SpeedUnit : BYTE {
      MPH,
      KMH,
      MPS
   };

   namespace Gameplay {
      namespace Player {
         static float* carSpeed = (float*)(0x5142C8 + 0x400000);

         static DWORD* getCarPowerBase() {
            return Memory::readPointer(0x5142D0, 1, 0x20);
         }
         static bool getCarPowerData(CarPowerData*& out_CarPowerData) {
            auto pBase = getCarPowerBase();
            if (!pBase || *pBase == 0xEEEEEEEE) {
               out_CarPowerData = nullptr;
               return false;
            }

            out_CarPowerData = (CarPowerData*)*pBase;
            return true;
         }
         static float getCarRPM(CarPowerData* cpd) {
            if (!cpd)
               return -1.0f;

            return max(
               (cpd->alignedCurrentRPM / 10000.0f) * cpd->maximumRPM,
               cpd->minimumRPM
            );
         }

         static float getCarSpeed(const SpeedUnit& targetUnit) {
            auto v = *carSpeed;
            switch (targetUnit) {
               case SpeedUnit::MPH:
                  return v * 2.23694f;
               case SpeedUnit::KMH:
                  return v * 3.6f;
            }
            return v;
         }

         static float* getCarNOSAsPointer() {
            auto pBase = Data::readPointerBase2();
            if (!pBase || !*pBase)
               return nullptr;

            return (float*)(*pBase + 0xA4);
         }
         static float getCarNOS() {
            auto p = getCarNOSAsPointer();
            return p ? *p : -1.0f;
         }

         static int getCashBalance() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return -1;

            return *(int*)(*pBase + 0xB4);
         }
      }
   }
}