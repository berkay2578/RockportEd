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
      /*         struct align          */ unsigned char __unk[0x4 * 3];
      float FogSkyColour[4];

      TimeOfDayLighting& operator=(const TimeOfDayLighting& newTimeOfDayLightingData) {
         memcpy_s(SpecularColour, sizeof(float) * 4, newTimeOfDayLightingData.SpecularColour, sizeof(float) * 4);
         memcpy_s(DiffuseColour, sizeof(float) * 4, newTimeOfDayLightingData.DiffuseColour, sizeof(float) * 4);
         memcpy_s(AmbientColour, sizeof(float) * 4, newTimeOfDayLightingData.AmbientColour, sizeof(float) * 4);
         memcpy_s(FogSkyColour, sizeof(float) * 4, newTimeOfDayLightingData.FogSkyColour, sizeof(float) * 4);
         memcpy_s(FogHazeColour, sizeof(float) * 4, newTimeOfDayLightingData.FogHazeColour, sizeof(float) * 4);
         memcpy_s(FixedFunctionSkyColour, sizeof(float) * 4, newTimeOfDayLightingData.FixedFunctionSkyColour, sizeof(float) * 4);
         FogDistanceScale   = newTimeOfDayLightingData.FogDistanceScale;
         FogSkyColourScale  = newTimeOfDayLightingData.FogSkyColourScale;
         FogHazeColourScale = newTimeOfDayLightingData.FogHazeColourScale;
         EnvSkyBrightness   = newTimeOfDayLightingData.EnvSkyBrightness;
         CarSpecScale       = newTimeOfDayLightingData.CarSpecScale;
         return *this;
      }
   };
   struct TimeOfDayLighting_WithOptionalParameters {
      /*         struct align          */ unsigned char __unk[0x4 * 6];
      TimeOfDayLighting* pLightingData;
      /*         struct align          */ unsigned char __unk2[0x4 * 9];
      float              FogInLightScatter;
      /*         struct align          */ unsigned char __unk3[0x4 * 2];
      float              FogSunFalloff;
   };
   struct TimeOfDay {
      float   SkyboxSpeedMultiplier;
      int32_t TimeOfDaySpeedMultiplier;
      float   TimeOfDayValue;
      float   SunDefaultOrbitAxisX;
      float   SunDefaultOrbitAxisY;
      /* struct align: Floats related to something, maybe the result of the lighting data mixing?? */ unsigned char __unk[0x4 * 44];
      TimeOfDayLighting_WithOptionalParameters* pTimeOfDayLightingInstanceWrapper;
      TimeOfDayLighting*                        pTimeOfDayLightingInstance;
   };

   namespace Gameplay {
      namespace World {
         // TODO: ToD Editor code here
      }
   }
}