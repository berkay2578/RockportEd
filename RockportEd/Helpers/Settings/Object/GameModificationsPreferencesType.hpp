/*
   MIT License
   
   Copyright (c) 2017-2018 Berkay Yiðit
   
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
      bool                      PauseGameplayOnFocusLoss = true;
      WindowedModeModifications WindowedModePreferences    = {};

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(PauseGameplayOnFocusLoss),
            CEREAL_NVP(WindowedModePreferences)
         );
      }
   };
}