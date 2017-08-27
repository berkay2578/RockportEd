#pragma once
#include "stdafx.h"
#include "Mods.h"
#include "Memory.h"
#include "D3D9Hook_Settings.h"

using std::map;
#include <vector>
using std::vector;

namespace Mods {
   namespace CarBytes {
      DWORD carbytesReadHook_Entry;
      DWORD carbytesReadHook_Ret;
      DWORD carBytes;

      void __declspec(naked) carbytesReadHook() {
         __asm {
            mov edi, [esp + 0x10]
            mov ebx, ecx
            pushad
         }
         if (D3D9HookSettings::Options::opt_CustomCarBytes) {
            /* rehook inside the loop, game loads bytes for eacH COMPONENT FUCKING FUCK YOU OLD EAGL
            __asm {
            push eax
            mov eax, [D3D9HookSettings::Options::opt_CustomCarBytesValue]
            mov[ebx], eax
            pop eax
            }*/
         }
         else {
            __asm {
               push eax
               mov eax, [ebx]
               mov[carBytes], eax
               pop eax
            }
            sprintf(D3D9HookSettings::Options::opt_CustomCarBytesValue, "%X", carBytes);
         }
         __asm {
            popad
            jmp carbytesReadHook_Ret
         }
      }

      void Init() {
         carbytesReadHook_Entry = Memory::makeAbsolute(0x16F2B3);
         carbytesReadHook_Ret   = Memory::makeAbsolute(0x16F2B9);
         Memory::writeJMP(carbytesReadHook_Entry, (DWORD)carbytesReadHook);
         Memory::writeNop(carbytesReadHook_Entry + 0x5, 1);
      }
   }
   namespace Camera {
      int* activeCamera                 = nullptr;
      map<int, map<char*, float*>> data ={};

      void Init() {
         while (!Memory::readPointer(0x51CF90, 2, 0x10, 0x8C)) {
            Sleep(100);
         }
         activeCamera = (int*)Memory::readPointer(0x51CF90, 2, 0x10, 0x8C);

         while (!Memory::readPointer(0x51DCC8, 1, 0xF4)) {
            Sleep(100);
         }
         DWORD camBase = *Memory::readPointer(0x51DCC8, 1, 0xF4);

         map<char*, float*> farCamera;
         farCamera["X"]        = (float*)(camBase + 0x2B148);
         farCamera["Z"]        = (float*)(camBase + 0x2B168);
         farCamera["Fov"]      = (float*)(camBase + 0x2B158);
         farCamera["HorAngle"] = (float*)(camBase + 0x2B138);
         farCamera["VerAngle"] = (float*)(camBase + 0x2B178);

         map<char*, float*> nearCamera;
         nearCamera["X"]        = (float*)(camBase + 0x2B058);
         nearCamera["Z"]        = (float*)(camBase + 0x2B078);
         nearCamera["Fov"]      = (float*)(camBase + 0x2B068);
         nearCamera["HorAngle"] = (float*)(camBase + 0x2B048);
         nearCamera["VerAngle"] = (float*)(camBase + 0x2B088);

         map<char*, float*> bumperCamera;
         bumperCamera["X"]        = (float*)(camBase + 0x2AFE0);
         bumperCamera["Z"]        = (float*)(camBase + 0x2B000);
         bumperCamera["Fov"]      = (float*)(camBase + 0x2AFF0);
         bumperCamera["HorAngle"] = (float*)(camBase + 0x2AFD0);
         bumperCamera["VerAngle"] = (float*)(camBase + 0x2B010);

         data[3] = farCamera;
         data[2] = nearCamera;
         data[0] = bumperCamera;
      }
   }
   namespace ReplaySystem {
      CarInfo* playerCarInfo;
      map<LPVOID, vector<float>> data ={};

      namespace Record {
         bool isRecording = false;
         UINT frameCount = 0;

         DWORD WINAPI record(LPVOID) {
            while (isRecording) {
               data[&playerCarInfo->x].push_back(playerCarInfo->x);
               data[&playerCarInfo->y].push_back(playerCarInfo->y);
               data[&playerCarInfo->z].push_back(playerCarInfo->z);

               data[&playerCarInfo->x_Rotation].push_back(playerCarInfo->x_Rotation);
               data[&playerCarInfo->y_Rotation].push_back(playerCarInfo->y_Rotation);
               data[&playerCarInfo->x_Lift].push_back(playerCarInfo->x_Lift);
               data[&playerCarInfo->y_Lift].push_back(playerCarInfo->y_Lift);

               data[&playerCarInfo->x_LiftForce].push_back(playerCarInfo->x_LiftForce);
               data[&playerCarInfo->y_LiftForce].push_back(playerCarInfo->y_LiftForce);

               data[&playerCarInfo->x_Velocity].push_back(playerCarInfo->x_Velocity);
               data[&playerCarInfo->y_Velocity].push_back(playerCarInfo->y_Velocity);
               data[&playerCarInfo->z_Velocity].push_back(playerCarInfo->z_Velocity);
               data[&playerCarInfo->angular_Velocity].push_back(playerCarInfo->angular_Velocity);

               frameCount++;
               Sleep(1);
            }

            data[&playerCarInfo->x_LiftForce].push_back(0.0f);
            data[&playerCarInfo->y_LiftForce].push_back(0.0f);

            data[&playerCarInfo->x_Velocity].push_back(0.0f);
            data[&playerCarInfo->y_Velocity].push_back(0.0f);
            data[&playerCarInfo->z_Velocity].push_back(0.0f);
            data[&playerCarInfo->angular_Velocity].push_back(0.0f);
            return TRUE;
         }
         void startRecording() {
            data[&playerCarInfo->x]              = vector<float>();
            data[&playerCarInfo->y]              = vector<float>();
            data[&playerCarInfo->z]              = vector<float>();
            data[&playerCarInfo->x_Rotation]     = vector<float>();
            data[&playerCarInfo->y_Rotation]     = vector<float>();
            data[&playerCarInfo->x_Lift]         = vector<float>();
            data[&playerCarInfo->y_Lift]         = vector<float>();

            data[&playerCarInfo->x_LiftForce] = vector<float>();
            data[&playerCarInfo->y_LiftForce] = vector<float>();

            data[&playerCarInfo->x_Velocity]          = vector<float>();
            data[&playerCarInfo->y_Velocity]          = vector<float>();
            data[&playerCarInfo->z_Velocity]          = vector<float>();
            data[&playerCarInfo->angular_Velocity]    = vector<float>();

            frameCount = 0;
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&record, 0, 0, 0);
         }
      }
      namespace Replay {
         bool isShowingReplay = false;
         bool isReplayPaused  = false;
         UINT frameNr         = 0;

         float origGravity, origMass;

         DWORD WINAPI replay(LPVOID) {
            do {
               if (isReplayPaused) {
                  playerCarInfo->x                = data[&playerCarInfo->x][frameNr];
                  playerCarInfo->y                = data[&playerCarInfo->y][frameNr];
                  playerCarInfo->z                = data[&playerCarInfo->z][frameNr];

                  playerCarInfo->x_Rotation       = data[&playerCarInfo->x_Rotation][frameNr];
                  playerCarInfo->y_Rotation       = data[&playerCarInfo->y_Rotation][frameNr];
                  playerCarInfo->x_Lift           = data[&playerCarInfo->x_Lift][frameNr];
                  playerCarInfo->y_Lift           = data[&playerCarInfo->y_Lift][frameNr];

                  playerCarInfo->x_LiftForce      = 0.0f;
                  playerCarInfo->y_LiftForce      = 0.0f;

                  playerCarInfo->x_Velocity       = 0.0f;
                  playerCarInfo->y_Velocity       = 0.0f;
                  playerCarInfo->z_Velocity       = 0.0f;

                  playerCarInfo->angular_Velocity = data[&playerCarInfo->angular_Velocity][frameNr];
                  Sleep(1);
                  playerCarInfo->angular_Velocity = 0.0f;
               }
               else {
                  for (frameNr; frameNr < Record::frameCount; frameNr++) {
                     playerCarInfo->x_LiftForce      = data[&playerCarInfo->x_LiftForce][frameNr];
                     playerCarInfo->y_LiftForce      = data[&playerCarInfo->y_LiftForce][frameNr];

                     playerCarInfo->x_Velocity       = data[&playerCarInfo->x_Velocity][frameNr];
                     playerCarInfo->y_Velocity       = data[&playerCarInfo->y_Velocity][frameNr];
                     playerCarInfo->z_Velocity       = data[&playerCarInfo->z_Velocity][frameNr];
                     playerCarInfo->angular_Velocity = data[&playerCarInfo->angular_Velocity][frameNr];

                     if (!isShowingReplay || isReplayPaused)
                        break;
                     Sleep(1);
                  }

                  playerCarInfo->x_LiftForce      = 0.0f;
                  playerCarInfo->y_LiftForce      = 0.0f;

                  playerCarInfo->x_Velocity       = 0.0f;
                  playerCarInfo->y_Velocity       = 0.0f;
                  playerCarInfo->z_Velocity       = 0.0f;
                  playerCarInfo->angular_Velocity = 0.0f;

                  if (!isReplayPaused)
                     break;
               }
               Sleep(1);
            } while (isShowingReplay);

            isShowingReplay = false;
            if (isReplayPaused) {
               isReplayPaused = false;
               changeReplayState();
            }
            return TRUE;
         }

         void changeReplayState() {
            if (isReplayPaused) {
               origGravity            = playerCarInfo->gravity;
               origMass               = playerCarInfo->mass;

               playerCarInfo->gravity = 0.0f;
               playerCarInfo->mass    = 0.0f;
            }
            else {
               playerCarInfo->gravity = origGravity;
               playerCarInfo->mass    = origMass;
            }
         }
         void startReplay() {
            frameNr = 0;
            playerCarInfo->x           = data[&playerCarInfo->x][frameNr];
            playerCarInfo->y           = data[&playerCarInfo->y][frameNr];
            playerCarInfo->z           = data[&playerCarInfo->z][frameNr];

            playerCarInfo->x_Rotation  = data[&playerCarInfo->x_Rotation][frameNr];
            playerCarInfo->y_Rotation  = data[&playerCarInfo->y_Rotation][frameNr];
            playerCarInfo->x_Lift      = data[&playerCarInfo->x_Lift][frameNr];
            playerCarInfo->y_Lift      = data[&playerCarInfo->y_Lift][frameNr];

            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&replay, 0, 0, 0);
         }
      }

      void Init() {
         playerCarInfo = (CarInfo*)Memory::makeAbsolute(0x5386C8);
      }
   }
   namespace NewHUD {
      namespace _internal {
         bool  isInit            = false;
         bool  isSuitable        = false;
         bool* isGameplayActive  = nullptr;
         bool* isGameLoaded      = nullptr;
         int*  gameplayState     = nullptr;

         float lastReadInSeconds = 10.0f;

         bool* isShowingOverlayOnHUD    = nullptr;
         bool* showGameGauges           = nullptr;
         DWORD* carPowerBase            = nullptr;
      }

      short* gear              = nullptr;
      float* rpm               = nullptr;
      float* perfectShiftRange = nullptr;
      float* maxRpm            = nullptr;
      float* speed             = nullptr;
      float* nos               = nullptr;
      float* speedbreaker      = nullptr;
      int*   money             = nullptr;

      bool getAddresses(const float& secondsSinceLastFrame) {
         if (!*_internal::isShowingOverlayOnHUD) {
            _internal::lastReadInSeconds += secondsSinceLastFrame;

            if (_internal::lastReadInSeconds > 2.0f) {
               _internal::lastReadInSeconds = 0.0f;

               _internal::showGameGauges = (bool*)Memory::readPointer(0x51CF90, 2, 0x10, 0x80);
               if (_internal::showGameGauges) {
                  *_internal::showGameGauges = false;

                  _internal::carPowerBase = Memory::readPointer(0x5142D0, 1, 0x20);
                  if (_internal::carPowerBase) {
                     gear              = (short*)(*_internal::carPowerBase + 0x8C);
                     rpm               = (float*)(*_internal::carPowerBase + 0x2C);
                     perfectShiftRange = (float*)(*_internal::carPowerBase + 0x44);
                     maxRpm            = (float*)(*_internal::carPowerBase + 0x3C);
                     speed             = (float*)Memory::readPointer(0x5352B0, 1, 0x11C);
                     nos               = (float*)Memory::readPointer(0x52D918, 4, 0x4C0, 0x4, 0x5C, 0xA4);
                     speedbreaker      = (float*)Memory::readPointer(0x589228, 1, 0x84);
                     money             = (int*)Memory::readPointer(0x51CF90, 2, 0x10, 0xB4);
                  }
               }
            }

            return gear && rpm && perfectShiftRange && maxRpm && speed && nos && speedbreaker && money;
         }
         return false;
      }

      bool confirmSuitableness(const float& secondsSinceLastFrame) {
         if (!_internal::isInit)
            return false;

         _internal::isSuitable = (*_internal::isGameplayActive && _internal::isGameLoaded) && (*_internal::gameplayState == 6);
         if (_internal::isSuitable)
            return getAddresses(secondsSinceLastFrame);

         return false;
      }

      float getRPM() {
         return max(((*rpm / 10000) * (*maxRpm)) + 200, 1000);
      }
      bool isInPerfectShiftRange() {
         if (getRPM() > 2000.0f)
            return (getRPM() + *perfectShiftRange) >= *maxRpm;
         return false;
      }
      bool isOverRevving() {
         return (getRPM() + 100.0f) >= *maxRpm;
      }

      void Init() {
         _internal::isGameplayActive = (bool*)Memory::makeAbsolute(0x4F40C4);
         _internal::isGameLoaded     = (bool*)Memory::makeAbsolute(0x51CD38);
         _internal::gameplayState    = (int*)Memory::makeAbsolute(0x525E90);
         _internal::isShowingOverlayOnHUD  = (bool*)Memory::makeAbsolute(0x51CAE4);

         _internal::isInit = true;
      }
   }

   DWORD WINAPI Init(LPVOID) {
      CarBytes::Init();
      Camera::Init();
      ReplaySystem::Init();
      NewHUD::Init();

      // just some settigs that I'm putting here since this is meant for private use anyways
      {
         *Camera::data[3]["X"]        = -6.376f;
         *Camera::data[3]["Z"]        = 1.56f;
         *Camera::data[3]["Fov"]      = 81.55f;
         *Camera::data[3]["HorAngle"] = 0.035f;
         *Camera::data[3]["VerAngle"] = 0.45f;

         Memory::writeRaw(0x37395A, false, 2, 0x38, 0xC0); // more traffic cars
         *(float*)Memory::readPointer(0x50DCC0, 3, 0x58, 0x3DC, 0x134) = 1.0f; // even more traffic cars
      }
      return TRUE;
   }
}