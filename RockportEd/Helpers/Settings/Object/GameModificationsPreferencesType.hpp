#pragma once
#include "stdafx.h"
// (De)Serialization
#include <cereal\archives\xml.hpp>

namespace Settings {
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
}