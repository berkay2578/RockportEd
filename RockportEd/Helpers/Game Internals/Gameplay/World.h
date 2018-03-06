#pragma once
#include <WinDef.h>

namespace GameInternals {
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
      BYTE  __unk[0x4 * 3];
      float FogSkyColour[4];
   };

   namespace Gameplay {
      namespace World {
         // TODO: ToD Editor code here
      }
   }
}