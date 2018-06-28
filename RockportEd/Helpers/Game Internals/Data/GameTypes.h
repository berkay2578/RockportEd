#pragma once
#include <WinDef.h>
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data::GameTypes {
   #pragma region Forward Declarations
      struct Vector3;
      struct Vector4;
      namespace UMath {
         struct Vector3;
         struct Vector4;
         struct Matrix4;
      }
      struct GIcon;
      struct GManager;
      struct ExtrapolatedCar;
      struct RigidBody;
      struct PVehicle;
   #pragma endregion
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

      struct Vector3 {
         float x, y, z;

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
         float x, y, z, w;

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

         struct Matrix4 {
            float m1_1;
            float m1_2;
            float m1_3;
            float m1_4;
            float m2_1;
            float m2_2;
            float m2_3;
            float m2_4;
            float m3_1;
            float m3_2;
            float m3_3;
            float m3_4;
            float m4_1;
            float m4_2;
            float m4_3;
            float m4_4;
         };
      }

      static DWORD stringhash32(const char* string) {
         return ((DWORD(__cdecl*)(const char*))0x5CC240)(string);
      }
      namespace Attrib {
         static DWORD StringToKey(const char* string) {
            return ((DWORD(__cdecl*)(const char*))0x454640)(string);
         }
      }

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

      private:
         static void ApplyPulsePatch() {
            Memory::writeRaw(0x57A226, true, 1, 0xEB);
         }
         static void RestorePulsePatch() {
            Memory::writeRaw(0x57A226, true, 1, 0x75);
         }

      public:
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
      struct GManager {
         static inline GManager** mObj = reinterpret_cast<GManager**>(0x51E00C + 0x400000);

         static GIcon* AllocIcon(GIconType iconType, Vector3* pCoordinates, const float unk1 = 0.0f, const bool unk2 = true) {
            return ((GIcon*(__thiscall*)(GManager*, GIconType, Vector3*, float, bool))0x5E9EC0)(*mObj, iconType, pCoordinates, unk1, unk2);
         }
         static void SpawnAllLoadedSectionIcons() {
            ((LPVOID(__thiscall*)(GManager*))0x5EDE20)(*mObj);
         }
         static void UnspawnAllIcons() {
            ((LPVOID(__thiscall*)(GManager*))0x5EA010)(*mObj);
         }
      };


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
      struct VehicleParams /* : Sim::Params */ {
         /* 0x0  */ DWORD                  typeName;
         /* 0x4  */ DWORD                  someFixedHash;
         /* 0x8  */ DWORD*                 pTypeName;
         /* 0xC  */ LPVOID __unk1; // even after hours of debugging, I don't know what this is
         /* 0x10 */ DriverClass            driverClass;
         /* 0x14 */ DWORD                  carHash;
         /* 0x18 */ UMath::Vector3*        pInitialRotation;
         /* 0x1C */ UMath::Vector3*        pInitialPosition;
         /* 0x20 */ FECustomizationRecord* pFECustomizationRecord;
         /* 0x24 */ DWORD __unk_AIRelated; // Set to AI managers, like &0x8918F0 for AICopManager or &0x8B09F4 for QuickGame
         /* 0x28 */ LPVOID __unk2; // related to Physics::*
         /* 0x2C */ DWORD  __unk_ImportanceRelated; // effects model loading delay, physics::tuning. it's a bit| enum

         VehicleParams() = default;
         void TypeName() {
            ((LPVOID(__cdecl*)(VehicleParams*))0x04040F0)(this);
         }
      };

      // RBVehicle
      // RBTractor
      struct RigidBodyData {
         /* +0x00 */ UMath::Vector4 Orientation;
         /* +0x10 */ UMath::Vector3 Position;
         /* +0x1C */ BYTE           VolatileStatus[4];
         /* +0x20 */ UMath::Vector3 LinearVelocity;
         /* +0x2C */ float          Mass;
         /* +0x30 */ UMath::Vector3 AngularVelocity;
         /* +0x3C */ float          OOMass; // CollisionMass
         /* +0x40 */ UMath::Vector3 InertiaTensor; // PrincipalInertia
         /* +0x4C */ unsigned char  __unk1[0x4];
         /* +0x50 */ UMath::Vector3 Force;
         /* +0x5C */ int8_t         NumContactPoints;
         /* +0x5D */ BYTE           Status;
         /* +0x5E */ BYTE           Index;
         /* +0x5F */ unsigned char  __unk2;
         /* +0x60 */ UMath::Vector3 Torque;
         /* +0x6C */ float          Radius;
         /* +0x70 */ UMath::Vector4 Rotation; // RightVector
         /* +0x80 */ UMath::Vector4 UpVector; // OrientToGround
         /* +0x90 */ UMath::Vector4 ForwardVector;
         /* +0xA0 */ unsigned char __unk3[0x4 * 4];

         bool CanCollideWithGround() {
            return Status = 0;
         }
         bool CanCollideWithRigidBodies() { // IsTriggering
            return !(VolatileStatus[1] & 0x01);
         }
         bool CanCollideWithWorld() {
            return VolatileStatus[1] & 0x40;
         }

         bool IsInGroundContact() {
            return NumContactPoints > 0;
         }
         bool IsModelling() {
            return Status != 2;
         }
      };
      // derived class from something, it is why the function pointers are different, just need to get a vfptr
      struct RigidBody {
         unsigned char __unk[0x30];
         /* +0x30 */ RigidBodyData** ppRigidBodyData;
         unsigned char __unk2[0xC];
         /* +0x3C */ DWORD*          pSomethingRelatedToPhysics;
         unsigned char __unk3[0x4C];
         /* +0x88 */ UMath::Vector3  GroundNormal;
         unsigned char __unk4[0x14];
         /* +0xA8 */ UMath::Vector3  CenterOfGravity;

         static inline RigidBody** instances = (RigidBody**)(0x52D0E8 + 0x400000);
         static inline RigidBodyData** mMaps = (RigidBodyData**)(0x5383B0 + 0x400000);

      private:
         RigidBody * getFunctionPointer(const int32_t& differenceOffset) {
            return (RigidBody*)(((unsigned char*)this) + differenceOffset);
         }

      public:
         void Accelerate(UMath::Vector3* pAxisFactor, float amount) {
            ((void(__thiscall*)(RigidBody*, UMath::Vector3*, float))0x699E10)(this, pAxisFactor, amount);
         }

         bool CanCollideWithObjects() {
            return ((bool(__thiscall*)(RigidBody*))0x699F30)(getFunctionPointer(-0x48));
         }

         void ConvertLocalToWorld(UMath::Vector3* out_pWorldVector3, bool normalize) {
            ((void(__thiscall*)(RigidBody*, UMath::Vector3*, bool))0x697570)(this, out_pWorldVector3, normalize);
         }
         void ConvertWorldToLocal(UMath::Vector3* out_pLocalVector3, bool normalize) {
            ((void(__thiscall*)(RigidBody*, UMath::Vector3*, bool))0x6975C0)(this, out_pLocalVector3, normalize);
         }

         // Velocity, Force and Torque damping
         void Damp(float amount) {
            ((void(__thiscall*)(RigidBody*, float))0x699FB0)(getFunctionPointer(0x8), amount);
         }

         UMath::Vector3* GetCenterOfGravity() {
            return ((UMath::Vector3*(__thiscall*)(RigidBody*))0x670DB0)(getFunctionPointer(0x8));
         }
         float GetGravity() {
            return ((float(__thiscall*)(RigidBody*))0x670DD0)(getFunctionPointer(0x8));
         }
         void GetMatrix4(UMath::Matrix4* out_pMatrix4) {
            ((void(__thiscall*)(RigidBody*, UMath::Matrix4*))0x6712A0)(this, out_pMatrix4);
         }
         float GetOrientToGround() {
            return ((float(__thiscall*)(RigidBody*))0x697150)(getFunctionPointer(0x8));
         }
         PVehicle* GetOwner() {
            return ((PVehicle*(__thiscall*)(RigidBody*))0x670DF0)(this);
         }
         DWORD GetSimableType() {
            return ((DWORD(__thiscall*)(RigidBody*))0x670E10)(this);
         }
         float GetSpeed() {
            return ((float(__thiscall*)(RigidBody*))0x6711B0)(this);
         }
         float GetSpeedXZ() {
            return ((float(__thiscall*)(RigidBody*))0x6711F0)(this);
         }
         DWORD* GetWCollider() {
            return ((DWORD*(__thiscall*)(RigidBody*))0x670E20)(this);
         }

         bool IsAnchored() {
            return ((bool(__thiscall*)(RigidBody*))0x670FE0)(getFunctionPointer(0x8));
         }
         bool IsAttachedToWorld() {
            return ((bool(__thiscall*)(RigidBody*))0x670FD0)(getFunctionPointer(0x8));
         }
         bool IsImmobile() {
            return ((bool(__thiscall*)(RigidBody*))0x698270)(getFunctionPointer(0x18));
         }
         bool IsModelling() {
            return ((bool(__thiscall*)(RigidBody*))0x670F40)(getFunctionPointer(0x8));
         }
         bool IsSleeping() {
            return ((bool(__thiscall*)(RigidBody*))0x670F60)(getFunctionPointer(0x8));
         }
         bool IsTriggering() {
            return ((bool(__thiscall*)(RigidBody*))0x697260)(getFunctionPointer(0x8));
         }
         bool HasHadCollision() {
            return ((bool(__thiscall*)(RigidBody*))0x670FA0)(getFunctionPointer(0x8));
         }
         bool HasHadWorldCollision() {
            return ((bool(__thiscall*)(RigidBody*))0x670FB0)(getFunctionPointer(0x8));
         }
         bool HasHadObjectCollision() {
            return ((bool(__thiscall*)(RigidBody*))0x670FC0)(getFunctionPointer(0x8));
         }

         void Reset() {
            ((void(__thiscall*)(RigidBody*))0x6A6D80)(getFunctionPointer(-0x48));
         }

         void SetAnimating(bool isAnimating) {
            ((void(__thiscall*)(RigidBody*, bool))0x6981F0)(getFunctionPointer(0x8), isAnimating);
         }
         void SetAttachedToWorld(bool isAttachedToWorld, float unk) {
            ((void(__thiscall*)(RigidBody*, bool, float))0x697020)(getFunctionPointer(0x8), isAttachedToWorld, unk);
         }
         void SetAnchored(bool isAnchored) {
            ((void(__thiscall*)(RigidBody*, bool))0x670FF0)(getFunctionPointer(0x8), isAnchored);
         }
         void SetGravity(float gravity) {
            *(float*)((unsigned char*)*pSomethingRelatedToPhysics + 0xF0) = gravity;
         }
         void SetMass(float mass) {
            ((void(__thiscall*)(RigidBody*, float))0x6970C0)(this, mass);
         }
         void SetModelling(const bool& isModelling) {
            if (isModelling)
               ((void(__thiscall*)(RigidBody*))0x697290)(getFunctionPointer(0x8)); // EnableModelling()
            else
               ((void(__thiscall*)(RigidBody*))0x697280)(getFunctionPointer(0x8)); // DisableModelling()
         }
         void SetTriggering(const bool& isTriggering) {
            if (isTriggering)
               ((void(__thiscall*)(RigidBody*))0x697270)(getFunctionPointer(0x8)); // EnableTriggering()
            else
               ((void(__thiscall*)(RigidBody*))0x697250)(getFunctionPointer(0x8)); // DisableTriggering()
         }
      };

      struct EventSequencer {
         struct IContext {
            virtual LPVOID SetDynamicData(const LPVOID EventSequencer__System, LPVOID EventDynamicData);
            DWORD _unk;
         };
      };
      struct Sim {
         struct ITaskable {
            virtual ~ITaskable();
            virtual void OnTask(LPVOID HSIMTASK__, float);
            unsigned char __unk[0x20];
         };
         struct Object : ITaskable {
            virtual ~Object();
            virtual void OnService(LPVOID, LPVOID);
         };
      };

      struct IAttachable {
         virtual ~IAttachable();
         virtual LPVOID Attach(LPVOID UTL__COM__IUnknown);
         virtual LPVOID Detach(LPVOID UTL__COM__IUnknown);
         virtual LPVOID IsAttached(const LPVOID UTL__COM__IUnknown);
         virtual LPVOID OnAttached(LPVOID IAttachable);
         virtual LPVOID OnDetached(LPVOID IAttachable);
         virtual LPVOID GetAttachments();
         unsigned char __unk[0x68];
      };
      struct IAttributeable {
         virtual LPVOID OnAttributeChange(const LPVOID Attrib__Collection, uint32_t);
         DWORD _unk;
      };
      struct IBody {
         virtual ~IBody();
         virtual LPVOID GetTransform(UMath::Matrix4&)       = 0;
         virtual LPVOID GetLinearVelocity(UMath::Vector3&)  = 0;
         virtual LPVOID GetAngularVelocity(UMath::Vector3&) = 0;
         virtual LPVOID GetDimension(UMath::Vector3&)       = 0;
         virtual LPVOID GetAttributes()                     = 0;
         virtual LPVOID GetWorldID()                        = 0;
         DWORD _unk;
      };
      struct IExplodeable {
         virtual LPVOID OnExplosion(const UMath::Vector3&, const UMath::Vector3&, float, LPVOID IExplosion);
      };
      struct ISimable {
         virtual ~ISimable();
         virtual LPVOID GetSimableType();
         virtual LPVOID Kill();
         virtual LPVOID Attach(LPVOID UTL__COM__IUnknown);
         virtual LPVOID Detach(LPVOID UTL__COM__IUnknown);
         virtual LPVOID GetAttachments();
         virtual LPVOID AttachEntity(LPVOID Sim__IEntity);
         virtual LPVOID DetachEntity();
         virtual LPVOID GetPlayer();
         virtual LPVOID IsPlayer();
         virtual LPVOID IsOwnedByPlayer();
         virtual LPVOID GetEntity();
         virtual LPVOID DebugObject();
         virtual LPVOID GetOwnerHandle();
         virtual LPVOID GetOwner();
         virtual LPVOID IsOwnedBy(ISimable*);
         virtual LPVOID SetOwnerObject(ISimable*);
         virtual LPVOID GetAttributes();
         virtual LPVOID GetWPos();
         virtual LPVOID _dupGetWPos();
         virtual RigidBody* GetRigidBody();
         virtual RigidBody* _dupGetRigidBody();
         virtual LPVOID IsRigidBodySimple();
         virtual LPVOID _dupIsRigidBodyComplex();
         virtual LPVOID GetPosition();
         virtual LPVOID GetTransform(UMath::Matrix4&);
         virtual LPVOID GetLinearVelocity(UMath::Vector3&);
         virtual LPVOID GetAngularVelocity(UMath::Vector3&);
         virtual LPVOID GetWorldID();
         virtual LPVOID GetEventSequencer();
         virtual LPVOID ProcessStimulus(uint32_t);
         virtual LPVOID GetModel() = 0;
         virtual LPVOID _dupGetModel() = 0;
         virtual LPVOID SetCausality(LPVOID HCAUSE__, float);
         virtual LPVOID GetCausality();
         virtual LPVOID GetCausalityTime();
         unsigned char __unk[8];
      };
      struct IVehicle {
         virtual ~IVehicle();
         virtual LPVOID GetSimable();
         virtual LPVOID _dupGetSimable();
         virtual LPVOID GetPosition();
         virtual LPVOID SetBehaviorOverride(DWORD, DWORD);
         virtual LPVOID RemoveBehaviorOverride(DWORD);
         virtual LPVOID CommitBehaviorOverrides();
         virtual LPVOID SetStaging(bool);
         virtual LPVOID IsStaging();
         virtual LPVOID Launch();
         virtual LPVOID GetPerfectLaunch();
         virtual LPVOID SetDriverStyle(LPVOID DriverStyle);
         virtual LPVOID GetDriverStyle();
         virtual LPVOID SetPhysicsMode(LPVOID PhysicsMode);
         virtual LPVOID GetPhysicsMode();
         virtual LPVOID GetModelType();
         virtual LPVOID IsSpooled();
         virtual LPVOID GetVehicleClass();
         virtual LPVOID GetVehicleAttributes();
         virtual LPVOID GetVehicleName();
         virtual LPVOID GetVehicleKey();
         virtual LPVOID SetDriverClass(DriverClass);
         virtual LPVOID GetDriverClass();
         virtual LPVOID IsLoading();
         virtual LPVOID GetOffscreenTime();
         virtual LPVOID GetOnScreenTime();
         virtual LPVOID SetVehicleOnGround(const UMath::Vector3&, const UMath::Vector3&);
         virtual LPVOID ForceStopOn(char);
         virtual LPVOID ForceStopOff(char);
         virtual LPVOID GetForceStop();
         virtual LPVOID InShock();
         virtual LPVOID IsDestroyed();
         virtual LPVOID Activate();
         virtual LPVOID Deactivate();
         virtual LPVOID IsActive();
         virtual LPVOID GetSpeedometer();
         virtual LPVOID GetSpeed();
         virtual LPVOID SetSpeed(float);
         virtual LPVOID GetAbsoluteSpeed();
         virtual LPVOID IsGlareOn(LPVOID VehicleFX__ID);
         virtual LPVOID GlareOn(LPVOID VehicleFX__ID);
         virtual LPVOID GlareOff(LPVOID VehicleFX__ID);
         virtual LPVOID IsCollidingWithSoftBarrier();
         virtual LPVOID GetAIVehiclePtr();
         virtual LPVOID GetSlipAngle();
         virtual LPVOID GetLocalVelocity();
         virtual LPVOID ComputeHeading(UMath::Vector3 *);
         virtual LPVOID IsAnimating();
         virtual LPVOID SetAnimating(bool);
         virtual LPVOID IsOffWorld();
         virtual LPVOID GetCustomizations();
         virtual LPVOID GetTunings();
         virtual LPVOID SetTunings(const LPVOID Physics__Tunings);
         virtual LPVOID GetPerformance(LPVOID& Physics__Info__Performance);
         virtual LPVOID sub_688220();
         DWORD _unk;
      };

      // name is made up, i don't know where these are from at all
      struct PhysicsObjectDetails {
         virtual void Reset();
         virtual void OnTaskSimulate(float) = 0;
         virtual void OnBehaviorChange(const DWORD);
         virtual void OnDebugDraw();
         DWORD _unk;
      };
      struct PhysicsObject : Sim::Object, PhysicsObjectDetails, ISimable, IBody, IAttachable {
         virtual ~PhysicsObject();
         virtual void OnService(LPVOID, LPVOID) override;

         // Sim::ITaskable overrides
         virtual void OnTask(LPVOID HSIMTASK__, float) override;
      };

      // sizeof(PVehicle) should be 0x1AC
      struct PVehicle : PhysicsObject, IVehicle, EventSequencer::IContext, IExplodeable, IAttributeable {
         virtual ~PVehicle();
         // PhysicsObject overrides
         virtual void Reset()                       override;
         virtual void OnTaskSimulate(float)         override;
         virtual void OnBehaviorChange(const DWORD) override;
         // EventSequencer::IContext overrides
         virtual LPVOID SetDynamicData(const LPVOID EventSequencer__System, LPVOID EventDynamicData);
         // Sim::ITaskable overrides
         virtual void OnTask(LPVOID HSIMTASK__, float) override;
         // IAttributeable overrides
         virtual LPVOID OnAttributeChange(const LPVOID Attrib__Collection, uint32_t);
         // IExplodeable overrides
         virtual LPVOID OnExplosion(const UMath::Vector3&, const UMath::Vector3&, float, LPVOID IExplosion);
         // IBody overrides
         virtual LPVOID GetTransform(UMath::Matrix4&)       override;
         virtual LPVOID GetLinearVelocity(UMath::Vector3&)  override;
         virtual LPVOID GetAngularVelocity(UMath::Vector3&) override;
         virtual LPVOID GetDimension(UMath::Vector3&)       override;
         virtual LPVOID GetAttributes()                     override;
         virtual LPVOID GetWorldID()                        override;
         // ISimable overrides
         virtual LPVOID Kill()              override;
         virtual LPVOID DebugObject()       override;
         virtual LPVOID GetPosition()       override;
         virtual LPVOID GetEventSequencer() override;
         virtual LPVOID GetModel()          override;
         virtual LPVOID _dupGetModel()      override;

         // 0xE4 EventSequencer

         PVehicle() = default;
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

            DWORD typeHash = stringhash32("PVehicle");

            // UTL::COM::Factory<Sim::Param,ISimable,UCrc32>::CreateInstance(UCrc32,Sim::Param)
            return ((PVehicle*(__cdecl*)(DWORD, DWORD, DWORD, VehicleParams*))0x41CB10)(typeHash, vehicleParams.typeName, vehicleParams.someFixedHash, &vehicleParams);
         }


      };
   }
}