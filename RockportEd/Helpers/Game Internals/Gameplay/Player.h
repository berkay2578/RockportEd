#pragma once
#include <WinDef.h>
#include "..\Data\Data.h"

namespace GameInternals {
   struct CarPhysicsTuning {
      float Steering     = 0.0f;
      float Handling     = 0.0f;
      float Brakes       = 0.0f;
      float RideHeight   = 0.0f;
      float Aerodynamics = 0.0f;
      float NOS          = 0.0f;
      float Turbo        = 0.0f;
   };
   struct CarPowerData {
      /*         struct align            */ BYTE __unk[11 * 0x4];
      float AlignedCurrentRPM;           // 0x2C
      /*         struct align            */ BYTE __unk2[1 * 0x4];
      float MinimumRPM;                  // 0x34
      float MaximumPossibleRPM;          // 0x38
      float MaximumRPM;                  // 0x3C
      float CurrentPedalInputPercentage; // 0x40
      /*         struct align            */ BYTE __unk3[19 * 0x4];
      int   CurrentGear;                 // 0x8C
      int   LastGear;                    // 0x90
   };

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
            static bool   getCarPowerData(CarPowerData*& out_CarPowerData) {
               auto pBase = getCarPowerBase();
               if (!pBase || *pBase == 0xEEEEEEEE) {
                  out_CarPowerData = nullptr;
                  return false;
               }

               out_CarPowerData = (CarPowerData*)*pBase;
               return true;
            }
            static float  getRPM(CarPowerData* cpd) {
               if (!cpd)
                  return -1.0f;

               return max(
                  (cpd->AlignedCurrentRPM / 10000.0f) * cpd->MaximumRPM,
                  cpd->MinimumRPM
               );
            }

            static float  getSpeed(const SpeedUnit& targetUnit) {
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
            static float  getNOSValue() {
               auto p = getNOSValueAsPointer();
               return p ? *p : -1.0f;
            }

            static void setCarPhysicsTuning(const CarPhysicsTuning* newCarPhysicsTuning) {
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