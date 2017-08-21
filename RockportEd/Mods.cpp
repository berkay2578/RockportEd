#pragma once
#include "stdafx.h"
#include "Mods.h"
#include "Memory.h"
#include "D3D9Hook_Settings.h"

using std::map;
#include <vector>
using std::vector;

// TODO: rewrite
namespace Mods {
   int* activeCamera = nullptr;
   map<int, map<char*, float*>> cameraData ={};

#pragma region Car bytes
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
#pragma endregion

#pragma region Player Replay
   struct CarInfo {
      float y_Lift;
      float y_Rotation;
      float x_Lift;
      float x_Rotation;

      float y;
      float z;
      float x;

      BYTE internalCheck_isPlayer1;
      BYTE internalCheck_isPlayer2;
      BYTE internalCheck_isPlayer3;
      BYTE internalCheck_isPlayer4;

      float y_Velocity;
      float z_Velocity;
      float x_Velocity;

      float gravity;

      float y_LiftForce;
      float angular_Velocity;
      float x_LiftForce;

      float mass;
   } *playerCarInfo;

   map<LPVOID, vector<float>> replayData ={};

#pragma region Record
   bool isRecording = false;
   UINT frameCount = 0;

   DWORD WINAPI record(LPVOID) {
      while (isRecording) {
         replayData[&playerCarInfo->x].push_back(playerCarInfo->x);
         replayData[&playerCarInfo->y].push_back(playerCarInfo->y);
         replayData[&playerCarInfo->z].push_back(playerCarInfo->z);

         replayData[&playerCarInfo->x_Rotation].push_back(playerCarInfo->x_Rotation);
         replayData[&playerCarInfo->y_Rotation].push_back(playerCarInfo->y_Rotation);
         replayData[&playerCarInfo->x_Lift].push_back(playerCarInfo->x_Lift);
         replayData[&playerCarInfo->y_Lift].push_back(playerCarInfo->y_Lift);

         replayData[&playerCarInfo->x_LiftForce].push_back(playerCarInfo->x_LiftForce);
         replayData[&playerCarInfo->y_LiftForce].push_back(playerCarInfo->y_LiftForce);

         replayData[&playerCarInfo->x_Velocity].push_back(playerCarInfo->x_Velocity);
         replayData[&playerCarInfo->y_Velocity].push_back(playerCarInfo->y_Velocity);
         replayData[&playerCarInfo->z_Velocity].push_back(playerCarInfo->z_Velocity);
         replayData[&playerCarInfo->angular_Velocity].push_back(playerCarInfo->angular_Velocity);

         frameCount++;
         Sleep(1);
      }

      replayData[&playerCarInfo->x_LiftForce].push_back(0.0f);
      replayData[&playerCarInfo->y_LiftForce].push_back(0.0f);

      replayData[&playerCarInfo->x_Velocity].push_back(0.0f);
      replayData[&playerCarInfo->y_Velocity].push_back(0.0f);
      replayData[&playerCarInfo->z_Velocity].push_back(0.0f);
      replayData[&playerCarInfo->angular_Velocity].push_back(0.0f);
      return TRUE;
   }
   void startRecording() {
      replayData[&playerCarInfo->x]              = vector<float>();
      replayData[&playerCarInfo->y]              = vector<float>();
      replayData[&playerCarInfo->z]              = vector<float>();
      replayData[&playerCarInfo->x_Rotation]     = vector<float>();
      replayData[&playerCarInfo->y_Rotation]     = vector<float>();
      replayData[&playerCarInfo->x_Lift]         = vector<float>();
      replayData[&playerCarInfo->y_Lift]         = vector<float>();

      replayData[&playerCarInfo->x_LiftForce] = vector<float>();
      replayData[&playerCarInfo->y_LiftForce] = vector<float>();

      replayData[&playerCarInfo->x_Velocity]          = vector<float>();
      replayData[&playerCarInfo->y_Velocity]          = vector<float>();
      replayData[&playerCarInfo->z_Velocity]          = vector<float>();
      replayData[&playerCarInfo->angular_Velocity]    = vector<float>();

      frameCount = 0;
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&record, 0, 0, 0);
   }
#pragma endregion

#pragma region Replay
   bool isShowingReplay = false;
   bool isReplayPaused  = false;
   UINT frameNr         = 0;

   float origGravity, origMass;

   DWORD WINAPI replay(LPVOID) {
      do {
         if (isReplayPaused) {
            playerCarInfo->x                = replayData[&playerCarInfo->x][frameNr];
            playerCarInfo->y                = replayData[&playerCarInfo->y][frameNr];
            playerCarInfo->z                = replayData[&playerCarInfo->z][frameNr];

            playerCarInfo->x_Rotation       = replayData[&playerCarInfo->x_Rotation][frameNr];
            playerCarInfo->y_Rotation       = replayData[&playerCarInfo->y_Rotation][frameNr];
            playerCarInfo->x_Lift           = replayData[&playerCarInfo->x_Lift][frameNr];
            playerCarInfo->y_Lift           = replayData[&playerCarInfo->y_Lift][frameNr];

            playerCarInfo->x_LiftForce      = 0.0f;
            playerCarInfo->y_LiftForce      = 0.0f;

            playerCarInfo->x_Velocity       = 0.0f;
            playerCarInfo->y_Velocity       = 0.0f;
            playerCarInfo->z_Velocity       = 0.0f;

            playerCarInfo->angular_Velocity = replayData[&playerCarInfo->angular_Velocity][frameNr];
            Sleep(1);
            playerCarInfo->angular_Velocity = 0.0f;
         }
         else {
            for (frameNr; frameNr < frameCount; frameNr++) {
               playerCarInfo->x_LiftForce      = replayData[&playerCarInfo->x_LiftForce][frameNr];
               playerCarInfo->y_LiftForce      = replayData[&playerCarInfo->y_LiftForce][frameNr];

               playerCarInfo->x_Velocity       = replayData[&playerCarInfo->x_Velocity][frameNr];
               playerCarInfo->y_Velocity       = replayData[&playerCarInfo->y_Velocity][frameNr];
               playerCarInfo->z_Velocity       = replayData[&playerCarInfo->z_Velocity][frameNr];
               playerCarInfo->angular_Velocity = replayData[&playerCarInfo->angular_Velocity][frameNr];

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
      playerCarInfo->x           = replayData[&playerCarInfo->x][frameNr];
      playerCarInfo->y           = replayData[&playerCarInfo->y][frameNr];
      playerCarInfo->z           = replayData[&playerCarInfo->z][frameNr];

      playerCarInfo->x_Rotation  = replayData[&playerCarInfo->x_Rotation][frameNr];
      playerCarInfo->y_Rotation  = replayData[&playerCarInfo->y_Rotation][frameNr];
      playerCarInfo->x_Lift      = replayData[&playerCarInfo->x_Lift][frameNr];
      playerCarInfo->y_Lift      = replayData[&playerCarInfo->y_Lift][frameNr];

      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&replay, 0, 0, 0);
   }
#pragma endregion
#pragma endregion
   // fix these damn pointer smh, fuck is + 0x2B148
   // read game code, there's [edi+0C] that's set from ecx for activeCam and blablablablalba
   DWORD WINAPI Init(LPVOID) {
   #pragma region Car Bytes
      carbytesReadHook_Entry = Memory::makeAbsolute(0x16F2B3);
      carbytesReadHook_Ret = Memory::makeAbsolute(0x16F2B9);
      Memory::writeJMP(carbytesReadHook_Entry, (DWORD)carbytesReadHook);
      Memory::writeNop(carbytesReadHook_Entry + 0x5, 1);
   #pragma endregion

   #pragma region Camera
      while (!Memory::readPointer(0x51CF90, 2, 0x10, 0x8C)) {
         Sleep(100);
      }
      activeCamera = (int*)Memory::readPointer(0x51CF90, 2, 0x10, 0x8C);

      while (!Memory::readPointer(0x51DCC8, 1, 0xF4)) {
         Sleep(100);
      }
      DWORD camBase = *Memory::readPointer(0x51DCC8, 1, 0xF4);

      map<char*, float*> farCamera;
      farCamera["X"] = (float*)(camBase + 0x2B148);
      farCamera["Z"] = (float*)(camBase + 0x2B168);
      farCamera["Fov"] = (float*)(camBase + 0x2B158);
      farCamera["HorAngle"] = (float*)(camBase + 0x2B138);
      farCamera["VerAngle"] = (float*)(camBase + 0x2B178);

      map<char*, float*> nearCamera;
      nearCamera["X"] = (float*)(camBase + 0x2B058);
      nearCamera["Z"] = (float*)(camBase + 0x2B078);
      nearCamera["Fov"] = (float*)(camBase + 0x2B068);
      nearCamera["HorAngle"] = (float*)(camBase + 0x2B048);
      nearCamera["VerAngle"] = (float*)(camBase + 0x2B088);

      map<char*, float*> bumperCamera;
      bumperCamera["X"] = (float*)(camBase + 0x2AFE0);
      bumperCamera["Z"] = (float*)(camBase + 0x2B000);
      bumperCamera["Fov"] = (float*)(camBase + 0x2AFF0);
      bumperCamera["HorAngle"] = (float*)(camBase + 0x2AFD0);
      bumperCamera["VerAngle"] = (float*)(camBase + 0x2B010);

      cameraData[3] = farCamera;
      cameraData[2] = nearCamera;
      cameraData[0] = bumperCamera;
   #pragma endregion

   #pragma region Replay System
      playerCarInfo = (CarInfo*)Memory::makeAbsolute(0x5386C8);
   #pragma endregion

   #ifdef NDEBUG
      *farCamera["X"] = -6.376f;
      *farCamera["Z"] = 1.56f;
      *farCamera["Fov"] = 81.55f;
      *farCamera["HorAngle"] = 0.035f;
      *farCamera["VerAngle"] = 0.45f;

      Memory::writeRaw(0x37395A, false, 2, 0x38, 0xC0); // more traffic cars
   #endif
      return TRUE;
   }
}