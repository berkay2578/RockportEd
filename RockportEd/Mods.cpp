#pragma once
#include "stdafx.h"
#include "Mods.h"
#include "Memory.h"
#include "D3D9Hook_Settings.h"

using std::map;
#include <vector>
using std::vector;

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
   struct PlayerInfo {
      DWORD x;
      DWORD y;
      DWORD z;

      DWORD rotX;
      DWORD rotY;
      DWORD liftX;
      DWORD liftY;
      DWORD forceLiftX;
      DWORD forceLiftY;

      DWORD velX;
      DWORD velY;
      DWORD velVert;
      DWORD velAng;
      //DWORD gravity;
   } playerInfo;

   map<DWORD, vector<float>> replayData ={};

#pragma region Record
   bool isRecording = false;
   UINT frameCount = 0;

   DWORD WINAPI record(LPVOID) {
      replayData[playerInfo.x][0] = *(float*)playerInfo.x;
      replayData[playerInfo.y][0] = *(float*)playerInfo.y;
      replayData[playerInfo.z][0] = *(float*)playerInfo.z;

      replayData[playerInfo.rotX][0]  = *(float*)playerInfo.rotX;
      replayData[playerInfo.rotY][0]  = *(float*)playerInfo.rotY;
      replayData[playerInfo.liftX][0] = *(float*)playerInfo.liftX;
      replayData[playerInfo.liftY][0] = *(float*)playerInfo.liftY;

      do {
         replayData[playerInfo.forceLiftX].push_back(*(float*)playerInfo.forceLiftX);
         replayData[playerInfo.forceLiftY].push_back(*(float*)playerInfo.forceLiftY);

         replayData[playerInfo.velX].push_back(*(float*)playerInfo.velX);
         replayData[playerInfo.velY].push_back(*(float*)playerInfo.velY);
         replayData[playerInfo.velVert].push_back(*(float*)playerInfo.velVert);
         replayData[playerInfo.velAng].push_back(*(float*)playerInfo.velAng);

         frameCount++;
         Sleep(1);
      } while (isRecording);

      replayData[playerInfo.forceLiftX].push_back(0.0f);
      replayData[playerInfo.forceLiftY].push_back(0.0f);

      replayData[playerInfo.velX].push_back(0.0f);
      replayData[playerInfo.velY].push_back(0.0f);
      replayData[playerInfo.velVert].push_back(0.0f);
      replayData[playerInfo.velAng].push_back(0.0f);
      return TRUE;
   }
   void startRecording() {
      replayData[playerInfo.x]          = vector<float>(1);
      replayData[playerInfo.y]          = vector<float>(1);
      replayData[playerInfo.z]          = vector<float>(1);
      replayData[playerInfo.rotX]       = vector<float>(1);
      replayData[playerInfo.rotY]       = vector<float>(1);
      replayData[playerInfo.liftX]      = vector<float>(1);
      replayData[playerInfo.liftY]      = vector<float>(1);

      replayData[playerInfo.forceLiftX] = vector<float>();
      replayData[playerInfo.forceLiftY] = vector<float>();

      replayData[playerInfo.velX]       = vector<float>();
      replayData[playerInfo.velY]       = vector<float>();
      replayData[playerInfo.velVert]    = vector<float>();
      replayData[playerInfo.velAng]     = vector<float>();

      frameCount = 0;
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&record, 0, 0, 0);
   }
#pragma endregion

#pragma region Replay
   bool isShowingReplay = false;
   UINT frameNr   = 0;

   DWORD WINAPI replay(LPVOID) {
      *(float*)playerInfo.x     = replayData[playerInfo.x][0];
      *(float*)playerInfo.y     = replayData[playerInfo.y][0];
      *(float*)playerInfo.z     = replayData[playerInfo.z][0];

      *(float*)playerInfo.rotX  = replayData[playerInfo.rotX][0];
      *(float*)playerInfo.rotY  = replayData[playerInfo.rotY][0];
      *(float*)playerInfo.liftX = replayData[playerInfo.liftX][0];
      *(float*)playerInfo.liftY = replayData[playerInfo.liftY][0];

      for (frameNr; frameNr < frameCount; frameNr++) {
         *(float*)playerInfo.forceLiftX = replayData[playerInfo.forceLiftX][frameNr];
         *(float*)playerInfo.forceLiftY = replayData[playerInfo.forceLiftY][frameNr];

         *(float*)playerInfo.velX       = replayData[playerInfo.velX][frameNr];
         *(float*)playerInfo.velY       = replayData[playerInfo.velY][frameNr];
         *(float*)playerInfo.velVert    = replayData[playerInfo.velVert][frameNr];
         *(float*)playerInfo.velAng     = replayData[playerInfo.velAng][frameNr];

         if (!isShowingReplay)
            break;
         Sleep(1);
      }

      *(float*)playerInfo.forceLiftX = 0.0f;
      *(float*)playerInfo.forceLiftY = 0.0f;

      *(float*)playerInfo.velX       = 0.0f;
      *(float*)playerInfo.velY       = 0.0f;
      *(float*)playerInfo.velVert    = 0.0f;
      *(float*)playerInfo.velAng     = 0.0f;
      return TRUE;
   }
   void startReplay() {
      frameNr = 0;
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&replay, 0, 0, 0);
   }
#pragma endregion
#pragma endregion
   // fix these damn pointer smh, fuck is + 0x2B148
   // read game code, there's [edi+0C] that's set from ecx for activeCam and blablablablalba
   DWORD WINAPI Init(LPVOID) {
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

   #pragma region Car Bytes
      carbytesReadHook_Entry = Memory::makeAbsolute(0x16F2B3);
      carbytesReadHook_Ret = Memory::makeAbsolute(0x16F2B9);
      Memory::writeJMP(carbytesReadHook_Entry, (DWORD)carbytesReadHook);
      Memory::writeNop(carbytesReadHook_Entry + 0x5, 1);
   #pragma endregion

      DWORD playerBase = Memory::makeAbsolute(0x5386C4);
      playerInfo.x = playerBase + 0x1C;
      playerInfo.y = playerBase + 0x14;
      playerInfo.z = playerBase + 0x18;
      playerInfo.rotX = playerBase + 0x10;
      playerInfo.rotY = playerBase + 0x8;
      playerInfo.liftX = playerBase + 0xC;
      playerInfo.liftY = playerBase + 0x4;

      playerInfo.forceLiftX = playerBase + 0x3C;
      playerInfo.forceLiftY = playerBase + 0x34;

      playerInfo.velX = playerBase + 0x2C;
      playerInfo.velY = playerBase + 0x24;
      playerInfo.velVert = playerBase + 0x28;
      playerInfo.velAng = playerBase + 0x38;
   }
}