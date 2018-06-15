#pragma once
#include "stdafx.h"
// Subtypes
#include "..\Object\ColoursType.hpp"
#include "..\Object\GameModificationsPreferencesType.hpp"
#include "..\Object\CameraInfoPresetType.hpp"
#include "..\Object\CarConfigurationPresetType.hpp"
#include "..\Object\TimeOfDayLightingPresetType.hpp"
// (De)Serialization
#include <cereal\types\map.hpp>
#include <cereal\archives\xml.hpp>

namespace Settings {
   class SettingsType {
   public:
      SettingsType() = default;

      bool isFirstTimeUser = true;

      Colours                                        colours          = {};
      GameModificationsPreferences                   preferences      = {};
      std::map<std::string, CameraPreset>            cameraPresets    = {};
      std::map<std::string, CarConfigurationPreset>  carConfigPresets = {};
      std::map<std::string, TimeOfDayLightingPreset> todPresets       = {};
   private:
      friend class cereal::access;
      template<class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(isFirstTimeUser),
            cereal::make_nvp("Colours", colours),
            cereal::make_nvp("GameModificationPreferences", preferences),
            cereal::make_nvp("CameraPresets", cameraPresets),
            cereal::make_nvp("CarConfigurationPresets", carConfigPresets),
            cereal::make_nvp("TimeOfDayLightingPresets", todPresets)
         );
      };
   };
}