#pragma once
#include "stdafx.h"
// Subtypes
#include "SettingsColoursType.hpp"
// (De)Serialization
#include <cereal\types\string.hpp>
#include <cereal\types\array.hpp>
#include <cereal\archives\xml.hpp>

class SettingsType {
public:
   SettingsType() = default;

   bool                isFirstTimeUser = true;
   SettingsColoursType colours         = { 0 };
private:
   friend class cereal::access;
   template<class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(isFirstTimeUser),
         cereal::make_nvp("Colours", colours)
      );
   };
};