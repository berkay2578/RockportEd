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
#include "Helpers\Game Internals\Gameplay\Camera.h"
// (De)Serialization
#include <cereal\details\traits.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

namespace Settings {
   struct CameraInfoPreset {
      float Stiffness[2];
      float Angle[2];
      float Lag[2];
      float FOV[2];
      float Height[2];
      float LateOffset[2];
      bool  Tilting[2];
      bool  Selectable[2];

      void operator=(const GameInternals::CameraInfo& newCameraInfo) {
         memcpy_s(Stiffness, sizeof(float) * 2, newCameraInfo.Stiffness, sizeof(float) * 2);
         memcpy_s(Angle, sizeof(float) * 2, newCameraInfo.Angle, sizeof(float) * 2);
         memcpy_s(Lag, sizeof(float) * 2, newCameraInfo.Lag, sizeof(float) * 2);
         memcpy_s(FOV, sizeof(float) * 2, newCameraInfo.FOV, sizeof(float) * 2);
         memcpy_s(Height, sizeof(float) * 2, newCameraInfo.Height, sizeof(float) * 2);
         memcpy_s(LateOffset, sizeof(float) * 2, newCameraInfo.LateOffset, sizeof(float) * 2);
         memcpy_s(Tilting, sizeof(bool) * 2, newCameraInfo.Tilting, sizeof(bool) * 2);
         memcpy_s(Selectable, sizeof(bool) * 2, newCameraInfo.Selectable, sizeof(bool) * 2);
      }
      void setTo(const GameInternals::CameraInfo* pNewCameraInfo) {
         *this = *pNewCameraInfo;
      }

      GameInternals::CameraInfo getGameInternalsCompliantData() {
         GameInternals::CameraInfo ret;
         memcpy_s(ret.Stiffness, sizeof(float) * 2, Stiffness, sizeof(float) * 2);
         memcpy_s(ret.Angle, sizeof(float) * 2, Angle, sizeof(float) * 2);
         memcpy_s(ret.Lag, sizeof(float) * 2, Lag, sizeof(float) * 2);
         memcpy_s(ret.FOV, sizeof(float) * 2, FOV, sizeof(float) * 2);
         memcpy_s(ret.Height, sizeof(float) * 2, Height, sizeof(float) * 2);
         memcpy_s(ret.LateOffset, sizeof(float) * 2, LateOffset, sizeof(float) * 2);
         memcpy_s(ret.Tilting, sizeof(bool) * 2, Tilting, sizeof(bool) * 2);
         memcpy_s(ret.Selectable, sizeof(bool) * 2, Selectable, sizeof(bool) * 2);

         return ret;
      }

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(Stiffness),
            CEREAL_NVP(Angle),
            CEREAL_NVP(Lag),
            CEREAL_NVP(FOV),
            CEREAL_NVP(Height),
            CEREAL_NVP(LateOffset),
            CEREAL_NVP(Tilting),
            CEREAL_NVP(Selectable)
         );
      }
   };
   struct CameraPreset {
      bool             JoyViewEnabled;
      bool             SpeedFOVEnabled;
      float            SpeedFOVScale;
      int16_t          NitrousFOVWidening;
      CameraInfoPreset InfoPreset;

      template <class Archive>
      void serialize(Archive& archive) {
         archive(
            CEREAL_NVP(JoyViewEnabled),
            CEREAL_NVP(SpeedFOVEnabled),
            CEREAL_NVP(SpeedFOVScale),
            CEREAL_NVP(NitrousFOVWidening),
            CEREAL_NVP(InfoPreset)
         );
      }
   };
}

// True std::map (de)serialization
namespace cereal {
   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void save(Archive& ar, const std::map<std::string, Settings::CameraPreset, C, A>& map) {
      for (const auto& item : map)
         ar(cereal::make_nvp(item.first, item.second));
   }

   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void load(Archive& ar, std::map<std::string, Settings::CameraPreset, C, A>& map) {
      map.clear();

      auto hint = map.begin();
      while (true) {
         const auto namePtr = ar.getNodeName();
         if (!namePtr)
            break;

         Settings::CameraPreset value; ar(value);
         hint = map.emplace_hint(hint, std::move(std::string(namePtr)), std::move(value));
      }
   }
}