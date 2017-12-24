#pragma once
#include "Type\SettingsType.hpp"

namespace Settings {
   extern SettingsType settingsType;

   bool loadSettings();
   bool saveSettings();
}