#pragma once
// (De)Serialization
#include <cereal\details\traits.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

struct WindowedModeModifications {
   bool EnableWindowedModeImprovements = true;
   bool DisableAltMenu                 = true;
   bool DisableWindowsMouse            = true;

   template <class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(EnableWindowedModeImprovements),
         CEREAL_NVP(DisableAltMenu),
         CEREAL_NVP(DisableWindowsMouse)
      );
   }
};

struct GameModificationsPreferences {
   bool                      PauseGameplayOnOnFocusLoss = true;
   WindowedModeModifications WindowedModePreferences    = {};

   template <class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(PauseGameplayOnOnFocusLoss),
         CEREAL_NVP(WindowedModePreferences)
      );
   }
};