/*
   MIT License
   
   Copyright (c) 2017-2018 Berkay Yiðit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#pragma once
#include "stdafx.h"
#include "Helpers\Game Internals\Gameplay\World.h"
// (De)Serialization
#include <cereal\details\traits.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

namespace Settings {
   struct TimeOfDayLightingDataPreset {
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
      float FogSkyColour[4];

      void operator=(const GameInternals::TimeOfDayLightingData& newTimeOfDayLightingData) {
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
      void setTo(const GameInternals::TimeOfDayLightingData* pNewTimeOfDayLightingData) {
         *this = *pNewTimeOfDayLightingData;
      }

      GameInternals::TimeOfDayLightingData getGameInternalsCompliantData() {
         GameInternals::TimeOfDayLightingData ret;
         memcpy_s(ret.SpecularColour, sizeof(float) * 4, SpecularColour, sizeof(float) * 4);
         memcpy_s(ret.DiffuseColour, sizeof(float) * 4, DiffuseColour, sizeof(float) * 4);
         memcpy_s(ret.AmbientColour, sizeof(float) * 4, AmbientColour, sizeof(float) * 4);
         memcpy_s(ret.FogHazeColour, sizeof(float) * 4, FogHazeColour, sizeof(float) * 4);
         memcpy_s(ret.FixedFunctionSkyColour, sizeof(float) * 4, FixedFunctionSkyColour, sizeof(float) * 4);
         memcpy_s(ret.FogSkyColour, sizeof(float) * 4, FogSkyColour, sizeof(float) * 4);
         ret.FogDistanceScale   = FogDistanceScale;
         ret.FogSkyColourScale  = FogSkyColourScale;
         ret.FogHazeColourScale = FogHazeColourScale;
         ret.EnvSkyBrightness   = EnvSkyBrightness;
         ret.CarSpecScale       = CarSpecScale;

         return ret;
      }

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(SpecularColour),
            CEREAL_NVP(DiffuseColour),
            CEREAL_NVP(AmbientColour),
            CEREAL_NVP(FogHazeColour),
            CEREAL_NVP(FixedFunctionSkyColour),
            CEREAL_NVP(FogDistanceScale),
            CEREAL_NVP(FogHazeColourScale),
            CEREAL_NVP(FogSkyColourScale),
            CEREAL_NVP(EnvSkyBrightness),
            CEREAL_NVP(CarSpecScale),
            CEREAL_NVP(FogSkyColour)
         );
      }
   };
   struct TimeOfDayLightingPreset {
      TimeOfDayLightingDataPreset LightingDataPreset;
      float FogInLightScatter;
      float FogSunFalloff;

      void operator=(GameInternals::TimeOfDayLighting& newTimeOfDayLighting) {
         LightingDataPreset = *newTimeOfDayLighting.pLightingData;
         FogInLightScatter  = newTimeOfDayLighting.FogInLightScatter;
         FogSunFalloff      = newTimeOfDayLighting.FogSunFalloff;
      }
      void setTo(GameInternals::TimeOfDayLighting* pNewTimeOfDayLighting) {
         *this = *pNewTimeOfDayLighting;
      }

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(LightingDataPreset),
            CEREAL_NVP(FogInLightScatter),
            CEREAL_NVP(FogSunFalloff)
         );
      }
   };
}

// True std::map (de)serialization
namespace cereal {
   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void save(Archive& ar, const std::map<std::string, Settings::TimeOfDayLightingPreset, C, A>& map) {
      for (const auto& item : map)
         ar(cereal::make_nvp(item.first, item.second));
   }

   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void load(Archive& ar, std::map<std::string, Settings::TimeOfDayLightingPreset, C, A>& map) {
      map.clear();

      auto hint = map.begin();
      while (true) {
         const auto namePtr = ar.getNodeName();
         if (!namePtr)
            break;

         Settings::TimeOfDayLightingPreset value; ar(value);
         hint = map.emplace_hint(hint, std::move(std::string(namePtr)), std::move(value));
      }
   }
}