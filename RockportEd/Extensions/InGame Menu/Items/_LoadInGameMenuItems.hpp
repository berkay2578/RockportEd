/*
   MIT License
   
   Copyright (c) Berkay Yigit
   
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
#include "Extensions\InGame Menu\InGameMenu.h"
#include "Debug.hpp"
#include "CarConfigurator.hpp"
#include "CameraEditor.hpp"
#include "GameplayOptions.hpp"
#include "CustomHUD.hpp"
#include "TimeOfDayEditor.hpp"

namespace Extensions {
   namespace InGameMenu {
      static void loadItemsToInGameMenu() {
         static Debug           debug           = {};
         static CarConfigurator carConfigurator = {};
         static CameraEditor    cameraEditor    = {};
         static GameplayOptions gameplayOptions = {};
         static CustomHUD       customHUD       = {};
         static TimeOfDayEditor todEditor       = {};

         items.push_back(&debug);
         items.push_back(&carConfigurator);
         items.push_back(&cameraEditor);
         items.push_back(&gameplayOptions);
         items.push_back(&customHUD);
         items.push_back(&todEditor);
         for (auto item : items) {
            if (!item->hasLoadedData)
               item->loadData();
         }
      }
   }
}