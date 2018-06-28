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
#include "Helpers\Game Internals\Data\GameTypes.h"
// (De)Serialization
#include <cereal\details\traits.hpp>
#include <cereal\types\map.hpp>
#include <cereal\archives\xml.hpp>

namespace Settings {
   struct CarPhysicsTuningPreset {
      float Steering     = 0.0f;
      float Handling     = 0.0f;
      float Brakes       = 0.0f;
      float RideHeight   = 0.0f;
      float Aerodynamics = 0.0f;
      float NOS          = 0.0f;
      float Turbo        = 0.0f;

      void setValuesFromGameInstance(GameInternals::Data::GameTypes::CarPhysicsTuning* pCarPhysicsTuningInstance) {
         Steering     = pCarPhysicsTuningInstance->Steering;
         Handling     = pCarPhysicsTuningInstance->Handling;
         Brakes       = pCarPhysicsTuningInstance->Brakes;
         RideHeight   = pCarPhysicsTuningInstance->RideHeight;
         Aerodynamics = pCarPhysicsTuningInstance->Aerodynamics;
         NOS          = pCarPhysicsTuningInstance->NOS;
         Turbo        = pCarPhysicsTuningInstance->Turbo;
      }

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(Steering),
            CEREAL_NVP(Handling),
            CEREAL_NVP(Brakes),
            CEREAL_NVP(RideHeight),
            CEREAL_NVP(Aerodynamics),
            CEREAL_NVP(NOS),
            CEREAL_NVP(Turbo)
         );
      }
   };

   struct CarConfigurationPreset {
      float Mass;
      float OOMass;
      CarPhysicsTuningPreset PhysicsTuning;

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(Mass),
            CEREAL_NVP(OOMass),
            CEREAL_NVP(PhysicsTuning)
         );
      }
   };
}

// True std::map (de)serialization
namespace cereal {
   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void save(Archive& ar, const std::map<std::string, Settings::CarConfigurationPreset, C, A>& map) {
      for (const auto& item : map)
         ar(cereal::make_nvp(item.first, item.second));
   }

   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void load(Archive& ar, std::map<std::string, Settings::CarConfigurationPreset, C, A>& map) {
      map.clear();

      auto hint = map.begin();
      while (true) {
         const auto namePtr = ar.getNodeName();
         if (!namePtr)
            break;

         Settings::CarConfigurationPreset value; ar(value);
         hint = map.emplace_hint(hint, std::move(std::string(namePtr)), std::move(value));
      }
   }
}