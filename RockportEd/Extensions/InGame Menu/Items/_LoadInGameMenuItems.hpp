#pragma once
#include "stdafx.h"
#include "Extensions\InGame Menu\InGameMenu.h"
#include "Debug.hpp"
#include "CarConfigurator.hpp"
#include "CameraEditor.hpp"
#include "GameplayOptions.hpp"
#include "LightingEditor.hpp"

namespace Extensions {
   namespace InGameMenu {
      static void loadItemsToInGameMenu() {
         static Debug           debug           = {};
         static CarConfigurator carConfigurator = {};
         static CameraEditor    cameraEditor    = {};
         static GameplayOptions gameplayOptions = {};
         static TimeOfDayEditor  lightingEditor  = {};

         items.push_back(&debug);
         items.push_back(&carConfigurator);
         items.push_back(&cameraEditor);
         items.push_back(&gameplayOptions);
         items.push_back(&lightingEditor);
         for (auto item : items) {
            if (!item->hasLoadedData)
               item->loadData();
         }
      }
   }
}