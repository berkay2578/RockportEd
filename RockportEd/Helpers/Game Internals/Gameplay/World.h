#pragma once

namespace GameInternals {
   struct TimeOfDayLightingData {
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

      void operator=(const TimeOfDayLightingData& newTimeOfDayLightingData) {
         memcpy_s(SpecularColour, sizeof(float) * 4, newTimeOfDayLightingData.SpecularColour, sizeof(float) * 4);
         memcpy_s(DiffuseColour, sizeof(float) * 4, newTimeOfDayLightingData.DiffuseColour, sizeof(float) * 4);
         memcpy_s(AmbientColour, sizeof(float) * 4, newTimeOfDayLightingData.AmbientColour, sizeof(float) * 4);
         memcpy_s(FogHazeColour, sizeof(float) * 4, newTimeOfDayLightingData.FogHazeColour, sizeof(float) * 4);
         memcpy_s(FixedFunctionSkyColour, sizeof(float) * 4, newTimeOfDayLightingData.FixedFunctionSkyColour, sizeof(float) * 4);
         memcpy_s(FogSkyColour, sizeof(float) * 4, newTimeOfDayLightingData.FogSkyColour, sizeof(float) * 4);
         FogDistanceScale   = newTimeOfDayLightingData.FogDistanceScale;
         FogSkyColourScale  = newTimeOfDayLightingData.FogSkyColourScale;
         FogHazeColourScale = newTimeOfDayLightingData.FogHazeColourScale;
         EnvSkyBrightness   = newTimeOfDayLightingData.EnvSkyBrightness;
         CarSpecScale       = newTimeOfDayLightingData.CarSpecScale;
      }
      void setTo(const TimeOfDayLightingData* pNewTimeOfDayLightingData) {
         *this = *pNewTimeOfDayLightingData;
      }

      void operator+=(const TimeOfDayLightingData& additionData) {
         float* arrDummy[2] = { (float*)this, (float*)&additionData };
         for (int i = 0; i < (0x80 / sizeof(float)); i++)
            arrDummy[0][i] += arrDummy[1][i];
      }
      TimeOfDayLightingData operator+(const TimeOfDayLightingData& additionData) {
         TimeOfDayLightingData result = *this;
         result += additionData;
         return result;
      }

      void operator*=(const float& multiplierValue) {
         float* arrDummy = (float*)this;
         for (int i = 0; i < (0x80 / sizeof(float)); i++)
            arrDummy[i] *= multiplierValue;
      }
      TimeOfDayLightingData operator*(const float& multiplierValue) {
         TimeOfDayLightingData result = *this;
         result *= multiplierValue;
         return result;
      }
   };
   struct TimeOfDayLighting {
      /*         struct align          */ unsigned char __unk[0x4 * 6];
      TimeOfDayLightingData* pLightingData;
      /*         struct align          */ unsigned char __unk2[0x4 * 9];
      float                  FogInLightScatter;
      /*         struct align          */ unsigned char __unk3[0x4 * 2];
      float                  FogSunFalloff;

      void operator=(const TimeOfDayLighting& newTimeOfDayLighting) {
         if (!pLightingData)
            pLightingData = newTimeOfDayLighting.pLightingData;
         else
            pLightingData->setTo(newTimeOfDayLighting.pLightingData);
         FogInLightScatter  = newTimeOfDayLighting.FogInLightScatter;
         FogSunFalloff      = newTimeOfDayLighting.FogSunFalloff;
      }
   };
   struct TimeOfDay {
      float   SkyboxSpeedMultiplier;
      int32_t TimeOfDaySpeedMultiplier;
      float   TimeOfDayValue;
      float   SunDefaultOrbitAxisX;
      float   SunDefaultOrbitAxisY;
      /* struct align: Floats related to something, maybe the result of the lighting data mixing?? */ unsigned char __unk[0x4 * 44];
      TimeOfDayLighting*     pTimeOfDayLighting;
      TimeOfDayLightingData* pTimeOfDayLightingData;
   };

   namespace Gameplay {
      namespace World {
         struct TimeOfDayLightingDefinition {
            DWORD              Hash;
            TimeOfDayLighting* pTimeOfDayLighting;
            DWORD unk;

            std::string getHashAsString() {
               char* chrHash = new char[10 + 1];
               sprintf_s(chrHash, 10 + 1, "0x%08X", Hash);
               std::string strHash(chrHash);

               delete[] chrHash;
               return strHash;
            }
         };
         struct TimeOfDayLightingBackup {
            TimeOfDayLighting     timeOfDayLighting     = { 0 };
            TimeOfDayLightingData timeOfDayLightingData = { 0 };

            TimeOfDayLightingBackup(const TimeOfDayLighting* _timeOfDayLighting, const TimeOfDayLightingData* _timeOfDayLightingData) {
               timeOfDayLighting               = *_timeOfDayLighting;
               timeOfDayLighting.pLightingData = &timeOfDayLightingData;
               timeOfDayLightingData.setTo(_timeOfDayLightingData);
            }
         };

         static TimeOfDay** ppTimeOfDay          = reinterpret_cast<TimeOfDay**>(0x5B392C + 0x400000);
         static bool*       timeOfDaySwapEnabled = (bool*)(0x4F86E8 + 0x400000);
         static std::vector<TimeOfDayLightingBackup*>     backups;
         static std::vector<TimeOfDayLightingDefinition*> lightingDefinitions;
         static bool loadLightingDefinitions() {
            if (lightingDefinitions.empty()) {
               TimeOfDayLightingDefinition** pArrTODLighting = (TimeOfDayLightingDefinition**)Memory::readPointer(0x5B392C, false, 3, 0xEC, 0x14, 0x1C);
               if (!pArrTODLighting)
                  return false;
               TimeOfDayLightingDefinition* arrTODLighting = *pArrTODLighting;

               for (;;) {
                  if (arrTODLighting->Hash == 0xAAAAAAAA)
                     break;
                  if (arrTODLighting->Hash) {
                     backups.push_back(new TimeOfDayLightingBackup(arrTODLighting->pTimeOfDayLighting, arrTODLighting->pTimeOfDayLighting->pLightingData));

                     lightingDefinitions.push_back(arrTODLighting);
                  }
                  arrTODLighting++;
               }
               return lightingDefinitions.size() > 0;
            }
            return true;
         }
         static void restoreAll() {
            if ((lightingDefinitions.size() > 0 || loadLightingDefinitions())
                && backups.size() == lightingDefinitions.size())
            {
               for (size_t i = 0; i < lightingDefinitions.size(); i++)
                  *lightingDefinitions[i]->pTimeOfDayLighting = backups[i]->timeOfDayLighting;
            }
         }

         static TimeOfDay* getTimeOfDay() {
            if (!ppTimeOfDay)
               return nullptr;

            return *ppTimeOfDay;
         }
         static TimeOfDayLighting* getCurrentTimeOfDayLighting() {
            TimeOfDay* pTimeOfDay = getTimeOfDay();
            if (pTimeOfDay)
               return pTimeOfDay->pTimeOfDayLighting;

            return nullptr;
         }
         static TimeOfDayLightingData* getCurrentTimeOfDayLightingData() {
            TimeOfDay* pTimeOfDay = getTimeOfDay();
            if (pTimeOfDay)
               return pTimeOfDay->pTimeOfDayLightingData;

            return nullptr;
         }

         static TimeOfDayLighting* getTimeOfDayLightingForHash(const DWORD& lightingHash) {
            if (lightingDefinitions.size() > 0 || loadLightingDefinitions()) {
               for (const auto* def : lightingDefinitions) {
                  if (def->Hash == lightingHash)
                     return def->pTimeOfDayLighting;
               }
            }

            return nullptr;
         }
         static TimeOfDayLightingData* getTimeOfDayLightingDataForHash(const DWORD& lightingHash) {
            TimeOfDayLighting* pTimeOfDayLighting = getTimeOfDayLightingForHash(lightingHash);
            if (pTimeOfDayLighting)
               return pTimeOfDayLighting->pLightingData;

            return nullptr;
         }

         static void mixLighting(const TimeOfDayLighting* pTODLighting_1, const float& intensity_1,
                                 const TimeOfDayLighting* pTODLighting_2, const float& intensity_2,
                                 TimeOfDayLighting* out_pTODLightingMixResult)
         {
            if (!pTODLighting_1 || !pTODLighting_2)
               out_pTODLightingMixResult = nullptr;

            *out_pTODLightingMixResult->pLightingData    = (*pTODLighting_1->pLightingData * intensity_1) + (*pTODLighting_2->pLightingData * intensity_2);
            out_pTODLightingMixResult->FogInLightScatter = (pTODLighting_1->FogInLightScatter * intensity_1) + (pTODLighting_2->FogInLightScatter * intensity_2);
            out_pTODLightingMixResult->FogSunFalloff     = (pTODLighting_1->FogSunFalloff * intensity_1) + (pTODLighting_2->FogSunFalloff * intensity_2);
         }
         static TimeOfDayLighting mixLighting(const TimeOfDayLighting& todLighting_1, const float& intensity_1,
                                              const TimeOfDayLighting& todLighting_2, const float& intensity_2)
         {
            TimeOfDayLighting     result     = { 0 };
            TimeOfDayLightingData resultData = { 0 };

            result.pLightingData = &resultData;
            mixLighting(&todLighting_1, intensity_1, &todLighting_2, intensity_2, &result);
            return result;
         }

         static void setTimeOfDaySwapEnabled(const bool& timeOfDaySwapEnabled) {
            Memory::writeRaw((DWORD)timeOfDaySwapEnabled, true, 1, timeOfDaySwapEnabled ? 0x01 : 0x00);
         }
      }
   }
} namespace TimeOfDayInternals = GameInternals::Gameplay::World;