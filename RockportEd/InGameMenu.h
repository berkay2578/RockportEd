#pragma once
#include <WinDef.h>

namespace Extensions {
   namespace InGameMenu {
      DWORD WINAPI Init(LPVOID);

      struct TimeOfDayLighting {
         float SpecularColour[4];
         float DiffuseColour[4];
         float AmbientColour[4];
         float FogHazeColour[4];
         float FixedFunctionSkyColour[4];
         float FogDistanceScale;
         float FogHazeColourScale;
         float FogSkyColourScale;
         float EnvSkyBrightness;
         float CarSpecScale;
         BYTE  unk[0x4 * 3];
         float FogSkyColour[4];
      };
      struct PhyicsTuning {
         float steering = 0.0f;
         float handling = 0.0f;
         float brakes = 0.0f;
         float rideheight = 0.0f;
         float aero = 0.0f;
         float nos = 0.0f;
         float turbo = 0.0f;
      };
   }
}