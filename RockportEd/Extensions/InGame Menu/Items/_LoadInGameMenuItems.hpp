#pragma once
#include "stdafx.h"
#include "Extensions\InGame Menu\InGameMenu.h"
#include "CameraEditor.hpp"
#include "GameplayOptions.hpp"
#include "LightingEditor.hpp"

namespace Extensions {
   namespace InGameMenu {
      static void loadItemsToInGameMenu() {
         static CameraEditor    cameraEditor    = {};
         static GameplayOptions gameplayOptions = {};
         static LightingEditor  lightingEditor  = {};

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