#pragma once
#include <WinDef.h>
#include "..\Data\Data.h"

namespace GameInternals {
   enum class SpeedUnit : BYTE {
      MPH,
      KMH,
      MPS
   };

   namespace Gameplay {
      namespace Player {
         static int getCashBalance() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return -1;

            return *(int*)(*pBase + 0xB4);
         }

         namespace Car {
            // TODO: Move this
            static float* carSpeed = (float*)(0x5142C8 + 0x400000);

            static DWORD* getCarPowerBase() {
               return Memory::readPointer(0x5142D0, 1, 0x20);
            }
            static bool getCarPowerData(Data::GameTypes::CarPowerData*& out_CarPowerData) {
               auto pBase = getCarPowerBase();
               if (!pBase || *pBase == 0xEEEEEEEE) {
                  out_CarPowerData = nullptr;
                  return false;
               }

               out_CarPowerData = (Data::GameTypes::CarPowerData*)*pBase;
               return true;
            }
            static float getRPM(Data::GameTypes::CarPowerData* cpd) {
               if (!cpd)
                  return -1.0f;

               return max(
                  (cpd->AlignedCurrentRPM / 10000.0f) * cpd->MaximumRPM,
                  cpd->MinimumRPM
               );
            }

            static float getSpeed(const SpeedUnit& targetUnit) {
               auto v = *carSpeed;
               switch (targetUnit) {
                  case SpeedUnit::MPH:
                     return v * 2.23694f;
                  case SpeedUnit::KMH:
                     return v * 3.6f;
                  default:
                     return v;
               }
            }

            static float* getNOSValueAsPointer() {
               auto pBase = Data::readPointerBase2();
               if (!pBase || !*pBase)
                  return nullptr;

               return (float*)(*pBase + 0xA4);
            }
            static float getNOSValue() {
               auto p = getNOSValueAsPointer();
               return p ? *p : -1.0f;
            }

            static void setCarPhysicsTuning(const Data::GameTypes::CarPhysicsTuning* newCarPhysicsTuning) {
               __asm {
                  push eax
                  push ebx
                  push ecx
                  push edx

                  push 0x10

                  mov edx, 0x227400 + 0x400000 // Event::__nw(uint)
                  call edx
                  add esp, 0x4

                  //mov ebx, [eax - 0x4]
                  mov ebx, [newCarPhysicsTuning]
                  push ebx // arg 2
                  push 0x0   // arg 1
                  mov ecx, eax // inner function variable

                  mov edx, 0x225840 + 0x400000 // ETuneVehicle::ETuneVehicle(uint,Physics::Tunings const *)
                  call edx

                  pop edx
                  pop ecx
                  pop ebx
                  pop eax
               }
            }
         }
      }
   }
}