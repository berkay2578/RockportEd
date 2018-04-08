#pragma once

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
   struct TimeOfDayLighting_Memory {
      unsigned char __unk[0x4 * 6];
      TimeOfDayLighting* LightingData;
      unsigned char __unk2[0x4 * 9];
      float FogInLightScatter;
      unsigned char __unk3[0x4 * 2];
      float FogSunFalloff;
   };
   struct TimeOfDay {
      float    SkyboxSpeedMultiplier;
      int32_t  TimeOfDaySpeedMultiplier;
      float    TimeOfDayValue;
      float    SunOrbitAngleInRads;
      float    SunOrbitPositionInRads;
      unsigned char __unk[0x4 * 44]; // Floats related to sun, shadows, skybox and maybe reflections 
      TimeOfDayLighting_Memory* TimeOfDayLightingMemoryInstance;
      TimeOfDayLighting*        TimeOfDayLightingInstance;
   };

   namespace Gameplay {
      namespace World {
         // TODO: ToD Editor code here
      }
   }
}