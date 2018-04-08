#pragma once

namespace GameInternals {
   struct TimeOfDay {
      float    SkyboxSpeedMultiplier;
      int32_t  SunSpeedMultiplier;
      float    ToDValue;
      float    SunAngleInRads;
      float    SunPositionInRads;
   };
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
      unsigned char  __unk[0x4 * 3];
      float FogSkyColour[4];

      TimeOfDayLighting& operator=(TimeOfDayLighting* todInstance) {
         memcpy_s(SpecularColour, sizeof(float) * 4, todInstance->SpecularColour, sizeof(float) * 4);
         memcpy_s(DiffuseColour, sizeof(float) * 4, todInstance->DiffuseColour, sizeof(float) * 4);
         memcpy_s(AmbientColour, sizeof(float) * 4, todInstance->AmbientColour, sizeof(float) * 4);
         memcpy_s(FogSkyColour, sizeof(float) * 4, todInstance->FogSkyColour, sizeof(float) * 4);
         memcpy_s(FogHazeColour, sizeof(float) * 4, todInstance->FogHazeColour, sizeof(float) * 4);
         memcpy_s(FixedFunctionSkyColour, sizeof(float) * 4, todInstance->FixedFunctionSkyColour, sizeof(float) * 4);
         FogDistanceScale   = todInstance->FogDistanceScale;
         FogSkyColourScale  = todInstance->FogSkyColourScale;
         FogHazeColourScale = todInstance->FogHazeColourScale;
         EnvSkyBrightness   = todInstance->EnvSkyBrightness;
         CarSpecScale       = todInstance->CarSpecScale;
         return *this;
      }
   };

   namespace Gameplay {
      namespace World {
         // TODO: ToD Editor code here
      }
   }
}