#pragma once
#include <WinDef.h>
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data::GameTypes {
      struct Vector3 {
         float x, y, z;

         Vector3(): x(0.0f), y(0.0f), z(0.0f) {}
         Vector3(const float _x, const float _y, const float _z) {
            x = _x;
            y = -_y;
            z = _z;
         }
         operator float*() {
            return reinterpret_cast<float*>(this);
         }
      };
      struct Vector4 {
         float x, y, z, w;

         Vector4(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
         Vector4(const float _x, const float _y, const float _z, const float _w) {
            x = _x;
            y = -_y;
            z = _z;
            w = _w;
         }
         operator float*() {
            return reinterpret_cast<float*>(this);
         }
      };
      namespace UMath {
         struct Vector3 {
            float y, z, x;

            Vector3(): x(0.0f), y(0.0f), z(0.0f) {}
            Vector3(const float _x, const float _y, const float _z) {
               x = _x;
               y = _y;
               z = _z;
            }
            operator float*() {
               return reinterpret_cast<float*>(this);
            }
         };
         struct Vector4 {
            float y, z, x, w;

            Vector4(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
            Vector4(const float _x, const float _y, const float _z, const float _w) {
               x = _x;
               y = _y;
               z = _z;
               w = _w;
            }
            operator float*() {
               return reinterpret_cast<float*>(this);
            }
         };

         static void ExtractZAxis(Vector4* pVector4, Vector3* pVector3) {
            ((LPVOID(__cdecl*)(Vector4*, Vector3*))0x5CA870)(pVector4, pVector3);
         }
      }

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

      namespace GenericFunctionsWrapper {
         static DWORD stringhash32(const char* string) {
            return ((DWORD(__cdecl*)(const char*))0x5CC240)(string);
         }
      }
      namespace AttribWrapper::Atrib {
         static DWORD StringToKey(const char* string) {
            return ((DWORD(__cdecl*)(const char*))0x454640)(string);
         }
      }

      namespace GIconWrapper {
         enum class GIconType : uint16_t {
            UNKNOWN                = 0x0,
            Event_Sprint           = 0x1,
            Event_Circuit          = 0x2,
            Event_Drag             = 0x3,
            Event_LapKO            = 0x4,
            Event_Toolbooth        = 0x5,
            Event_SpeedtrapRace    = 0x6,
            Event_Challenge        = 0x7,
            SafeHouse              = 0x8,
            CarLot                 = 0x9,
            Shop                   = 0xA,
            Pursuit_HidingSpot     = 0xB,
            Pursuit_PursuitBreaker = 0xC,
            SpeedTrap              = 0xD,
            SpeedTrap_Duplicate    = 0xE,
            GoalMarker             = 0xF
         };
         struct GIcon {
            GIconType iconType;
            uint16_t  showType;
            unsigned char _unk[0x8];
            DWORD     emitterHash;
            Vector3   coordinates;

            ~GIcon() {
               ((LPVOID(__thiscall*)(GIcon*))0x5EBED0)(this);
            }

            DWORD FindSection() {
               return ((DWORD(__thiscall*)(GIcon*))0x5DE590)(this);
            }
            void CreateGeometry(DWORD unknown) {
               // dword_9B38A8 = WorldModelSlotPool
               ((LPVOID(__thiscall*)(GIcon*, DWORD))0x5DE690)(this, unknown);
            }
            void CreateParticleEffect(DWORD effectHash) {
               // 5DE5F0 =  GIcon::NotifyEmitterGroupDelete(GIcon*, EmitterGroup*)
               ((LPVOID(__thiscall*)(GIcon*, DWORD))0x5DE610)(this, effectHash);
            }
            void SetPosition() {
               ((LPVOID(__thiscall*)(GIcon*))0x5E5A90)(this);
            }
            void Spawn() {
               //dword_91E018 = sNumSpawned
               ((LPVOID(__thiscall*)(GIcon*))0x5EC270)(this);
            }
            void Unspawn() {
               ((LPVOID(__thiscall*)(GIcon*))0x5E5A00)(this);
            }
            void SnapToGround() {
               ((LPVOID(__thiscall*)(GIcon*))0x5EA0F0)(this);
            }

            void Show() {
               showType |= 0x3;
            }
            void Hide() {
               showType ^= 0x3;
            }
            void HideUntilRespawn() {
               Hide();
               showType |= 0x42;
            }
            void CancelHideUntilRespawn() {
               showType ^= 0x42;
            }

            static void ApplyPulsePatch() {
               Memory::writeRaw(0x57A226, true, 1, 0xEB);
            }
            static void RestorePulsePatch() {
               Memory::writeRaw(0x57A226, true, 1, 0x75);
            }
            void StartPulsing() {
               if (iconType != GIconType::GoalMarker)
                  ApplyPulsePatch();
               showType |= 0x80;
            }
            void StopPulsing() {
               showType ^= 0x80;
               RestorePulsePatch();
            }
         };
      }
      struct GManager {
         static inline GManager** mObj = reinterpret_cast<GManager**>(0x51E00C + 0x400000);

         static GIconWrapper::GIcon* AllocIcon(GIconWrapper::GIconType iconType, Vector3* coordinates, const float _unk1, const bool _unk2) {
            return ((GIconWrapper::GIcon*(__thiscall*)(GManager*, GIconWrapper::GIconType, Vector3*, float, bool)) 0x5E9EC0)(*mObj, iconType, coordinates, _unk1, _unk2);
         }
         static void SpawnAllLoadedSectionIcons() {
            ((LPVOID(__thiscall*)(GManager*))0x5EDE20)(*mObj);
         }
         static void UnspawnAllIcons() {
            ((LPVOID(__thiscall*)(GManager*))0x5EA010)(*mObj);
         }
      };

      namespace ExtrapolatedCarWrapper {
         struct ExtrapolatedCar {
            unsigned char  __unk[0x4 * 0xC];
            UMath::Vector4 vector4;
            unsigned char  __unk2[0xF64];

            ExtrapolatedCar(unsigned int unk = 0) {
               ((LPVOID(__thiscall*)(ExtrapolatedCar*, DWORD))0x74D510)(this, unk);
            }
            void ImportSimable(LPVOID pISimable, float unk1, float unk2) {
               ((LPVOID(__thiscall*)(ExtrapolatedCar*, LPVOID, float, float))0x75E210)(this, pISimable, unk1, unk2);
            }
            void ExtractSimable(LPVOID pISimable) {
               ((LPVOID(__thiscall*)(ExtrapolatedCar*, LPVOID))0x760700)(this, pISimable);
            }

            LPVOID Spawn(DWORD carModelHash) {
               return ((LPVOID(__thiscall*)(ExtrapolatedCar*, DWORD))0x74D430)(this, carModelHash);
            }
         };
      }
      namespace PVehicleWrapper {
         enum class DriverClass : uint32_t {
            Default = 0, // Used for the player
            Traffic = 1,
            Cop     = 2, // With cop patrolling minimap marker
            Racer   = 3, // With racer minimap marker and racer AI
            Dummy   = 6
         };

         struct FECustomizationRecord {
            unsigned char __parts[0x198];
         };

         struct VehicleParams {
            DWORD                  typeName;
            DWORD                  someFixedHash;
            DWORD*                 pTypeName;
            DWORD __unk1;
            DriverClass            driverClass;
            DWORD                  carHash;
            UMath::Vector3*        pInitialRotation;
            UMath::Vector3*        pInitialPosition;
            FECustomizationRecord* pFECustomizationRecord;
            LPVOID __unk_pointer; // related to some call, game attempts to call [+0x4] if not nullptr
            DWORD  __unk2;
            DWORD  __unk3; // set to 2
            DWORD  __unk4; // end of struct

            VehicleParams() = default;
            void TypeName() {
               ((LPVOID(__cdecl*)(VehicleParams*))0x04040F0)(this);
            }
         };

         struct PVehicle {
            PVehicle() = default;

            static PVehicle* Create(DriverClass driverClass, DWORD carModelHash, FECustomizationRecord* pFECustomizationRecord,
                                    UMath::Vector3& initialRotation, UMath::Vector3& initialPosition) {
               if (!carModelHash) {
                  char** pSkipFEPlayer2CarName = reinterpret_cast<char**>(0x4F86AC + 0x400000);
                  carModelHash = AttribWrapper::Atrib::StringToKey(*pSkipFEPlayer2CarName);
               }

               VehicleParams vehicleParams = { 0 };
               vehicleParams.TypeName();
               vehicleParams.someFixedHash          = 0x0A6B47FAC;
               vehicleParams.pTypeName              = &vehicleParams.typeName;
               vehicleParams.driverClass            = driverClass;
               vehicleParams.carHash                = carModelHash;
               vehicleParams.pInitialRotation       = &initialRotation;
               vehicleParams.pInitialPosition       = &initialPosition;
               vehicleParams.pFECustomizationRecord = pFECustomizationRecord;
               vehicleParams.__unk3                 = 2;

               DWORD typeHash = GenericFunctionsWrapper::stringhash32("PVehicle");
               DWORD* mHead   = reinterpret_cast<DWORD*>(0x92C66C); // UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::Prototype::mHead
               if (!mHead)
                  return NULL;

               mHead = reinterpret_cast<DWORD*>(*mHead);
               /* MEMORY MAP
                * TYPE HASH (4 BYTES) | CONSTRUCTOR FUNCTION POINTER (4 BYTES) | NEXT LIST ENTRY POINTER (4 BYTES)
                */
               while (typeHash) { // find the constructor that corresponds to the hash value, in this case for 'PVehicle'
                  if (*mHead == typeHash) {
                     break;
                  } else {
                     mHead = reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(mHead + 2));
                     if (!mHead)
                        break;
                  }
               }

               return reinterpret_cast<PVehicle*>(
                  ((unsigned char*(__cdecl*)(VehicleParams, DWORD))*(mHead + 1))(vehicleParams, vehicleParams.someFixedHash)
                  - 0x44);
            }
         };
      }
   }
}