#pragma once
#include "stdafx.h"
// Subtypes
#include "SettingsColoursType.hpp"
#include "CameraInfoPresetType.hpp"
#include "TimeOfDayLightingPresetType.hpp"
// (De)Serialization
#include <cereal\types\map.hpp>
#include <cereal\archives\xml.hpp>

class SettingsType {
public:
   SettingsType() = default;

   bool isFirstTimeUser = true;

   SettingsColoursType colours = {};
   std::map<std::string, CameraInfoPreset>        camPresets = {};
   std::map<std::string, TimeOfDayLightingPreset> todPresets = {};
private:
   friend class cereal::access;
   template<class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(isFirstTimeUser),
         cereal::make_nvp("Colours", colours),
         cereal::make_nvp("CameraPresets", camPresets),
         cereal::make_nvp("TimeOfDayLightingPresets", todPresets)
      );
   };
};