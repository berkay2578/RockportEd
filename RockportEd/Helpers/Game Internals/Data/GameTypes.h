#pragma once
#include <WinDef.h>
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data::GameTypes {
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
      struct ObjectData {
         float y_Lift;
         float y_Rotation;
         float x_Lift;
         float x_Rotation;

         float y_Position;
         float z_Position;
         float x_Position;

         unsigned char unk[4];

         float y_Velocity;
         float z_Velocity;
         float x_Velocity;

         float Mass;

         float y_LiftForce;
         float angular_Velocity;
         float x_LiftForce;

         float GForce;

         // TODO: Add definitions
         unsigned char NOT_UNKNOWN_BUT_PLACEHOLDER_FOR_STRUCT_ALIGN[0x4 * 0x1C];
      };

      struct Vector3 {
         float x, y, z;

         Vector3(): x(0.0f), y(0.0f), z(0.0f) {}
         Vector3(const float _x, const float _y, const float _z) {
            x = _x;
            y = _y;
            z = _z;
         }
         //Vector3(const UMath::Vector3& uVector3) {
         //   x = uVector3.x;
         //   y = -uVector3.y;
         //   z = uVector3.z;
         //}

         operator float*() {
            return reinterpret_cast<float*>(this);
         }
      };
      struct Vector4 {
         float x, y, z, w;

         Vector4(): x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
         Vector4(const float _x, const float _y, const float _z, const float _w) {
            x = _x;
            y = _y;
            z = _z;
            w = _w;
         }
         //Vector4(const UMath::Vector4& uVector4) {
         //   x = uVector4.x;
         //   y = -uVector4.y;
         //   z = uVector4.z;
         //   w = uVector4.w;
         //}

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
            Vector3(const GameTypes::Vector3& vector3) {
               x = vector3.x;
               y = -vector3.y;
               z = vector3.z;
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
            Vector4(const GameTypes::Vector4& vector4) {
               x = vector4.x;
               y = -vector4.y;
               z = vector4.z;
               w = vector4.w;
            }

            operator float*() {
               return reinterpret_cast<float*>(this);
            }
         };

         static void ExtractZAxis(Vector4* pVector4, Vector3* pVector3) {
            ((LPVOID(__cdecl*)(Vector4*, Vector3*))0x5CA870)(pVector4, pVector3);
         }
      }

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
            return ((GIconWrapper::GIcon*(__thiscall*)(GManager*, GIconWrapper::GIconType, Vector3*, float, bool))0x5E9EC0)(*mObj, iconType, coordinates, _unk1, _unk2);
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
            LPVOID __unk1; // even after hours of debugging, I don't know what this is
            DriverClass            driverClass;
            DWORD                  carHash;
            UMath::Vector3*        pInitialRotation;
            UMath::Vector3*        pInitialPosition;
            FECustomizationRecord* pFECustomizationRecord;
            DWORD __unk_AIRelated; // Set to AI managers, like &0x8918F0 for AICopManager or &0x8B09F4 for QuickGame
            LPVOID __unk2; // related to Physics::*, maybe Physics::Tuning?
            DWORD  __unk_ImportanceRelated; // effects model loading delay, physics::tuning. it's a bit| enum

            VehicleParams() = default;
            void TypeName() {
               ((LPVOID(__cdecl*)(VehicleParams*))0x04040F0)(this);
            }
         };

         struct PVehicle {
            PVehicle() = default;

            ObjectData* getObjectData() {
               return *(ObjectData**)Memory::readPointer((DWORD)this + 0x78, true, 1, 0x30);
            }

            static PVehicle* Create(DriverClass driverClass, DWORD carModelHash, FECustomizationRecord* pFECustomizationRecord,
                                    UMath::Vector3& initialRotation, UMath::Vector3& initialPosition) {
               if (!carModelHash)
                  return nullptr;

               VehicleParams vehicleParams = { 0 };
               vehicleParams.TypeName();
               vehicleParams.someFixedHash           = 0x0A6B47FAC;
               vehicleParams.pTypeName               = &vehicleParams.typeName;
               //vehicleParams.__unk1                  = &(((DWORD64*)(*(DWORD*)0x989080))[11]);// ? &(((DWORD64*)(*(DWORD*)0x989080))[a]) : nullptr;
               vehicleParams.driverClass             = driverClass;
               vehicleParams.carHash                 = carModelHash;
               vehicleParams.pInitialRotation        = &initialRotation;
               vehicleParams.pInitialPosition        = &initialPosition;
               vehicleParams.pFECustomizationRecord  = pFECustomizationRecord;
               vehicleParams.__unk_ImportanceRelated = 0xA;

               DWORD typeHash = GenericFunctionsWrapper::stringhash32("PVehicle");
               // UTL::COM::Factory<Sim::Param,ISimable,UCrc32>::CreateInstance(UCrc32,Sim::Param)
               auto constructorCallResult = ((unsigned char*(__cdecl*)(DWORD, DWORD, DWORD, VehicleParams*))0x41CB10)(typeHash, vehicleParams.typeName, vehicleParams.someFixedHash, &vehicleParams);

               /* manual method, good for debugging
               DWORD* mHead    = reinterpret_cast<DWORD*>(0x92C66C); // UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::Prototype::mHead
               if (!mHead)
                  return nullptr;

               mHead = reinterpret_cast<DWORD*>(*mHead);
               /* MEMORY MAP
                * TYPE HASH (4 BYTES) | CONSTRUCTOR FUNCTION POINTER (4 BYTES) | NEXT LIST ENTRY POINTER (4 BYTES)
                *//*
               while (typeHash) { // find the constructor that corresponds to the hash value, in this case for 'PVehicle'
                  if (*mHead == typeHash) {
                     break;
                  } else {
                     mHead = reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(mHead + 2));
                     if (!mHead)
                        break;
                  }
               }

               auto constructorCallResult = ((unsigned char*(__cdecl*)(DWORD, DWORD, VehicleParams*))*(mHead + 1))(vehicleParams.typeName, vehicleParams.someFixedHash, &vehicleParams);
               */

               if (constructorCallResult)
                  return reinterpret_cast<PVehicle*>(constructorCallResult - 0x2C);
               return nullptr;
            }
         };
      }
   }
}