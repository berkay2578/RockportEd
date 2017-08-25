#pragma once
#include "stdafx.h"
#include <map>

namespace Mods {
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
   };

   namespace Camera {
      extern int* activeCamera;
      extern std::map<int, std::map<char*, float*>> data;
   }
   namespace ReplaySystem {
      extern CarInfo* playerCarInfo;

      namespace Record {
         extern UINT frameCount;
         extern bool isRecording;
         extern void startRecording();
      }
      namespace Replay {
         extern bool isShowingReplay;
         extern bool isReplayPaused;
         extern UINT frameNr;
         extern void changeReplayState();
         extern void startReplay();
      }
   }
   namespace NewHUD {
      extern bool   isSuitable;

      extern short* gear;
      extern float* rpm;
      extern float* maxRpm;
      extern float* speed;
      extern float* nos;
      extern float* speedbreaker;
      extern int*   money;

      float getRPM();
      bool  isInPerfectShiftRange();
      bool  isOverRevving();
   }

   DWORD WINAPI Init(LPVOID);
}