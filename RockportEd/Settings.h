#pragma once
#include "SettingsType.hpp"

namespace Settings {
   extern SettingsType settingsType;

   bool loadSettings();
   bool saveSettings();
}