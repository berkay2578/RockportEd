#pragma once
#include "Helpers\Game Internals\Gameplay\Camera.h"
// (De)Serialization
#include <cereal\details\traits.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

struct CameraInfoPreset {
   float stiffness[2];
   float angle[2];
   float lag[2];
   float fov[2];
   float height[2];
   float lateOffset[2];
   bool tilting[2];
   bool selectable[2];

   void loadToCameraInfo(GameInternals::CameraInfo* cameraInfoInstance) {
      memcpy_s(cameraInfoInstance->stiffness, sizeof(float) * 2, stiffness, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->angle, sizeof(float) * 2, angle, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->lag, sizeof(float) * 2, lag, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->fov, sizeof(float) * 2, fov, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->height, sizeof(float) * 2, height, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->lateOffset, sizeof(float) * 2, lateOffset, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->tilting, sizeof(bool) * 2, tilting, sizeof(bool) * 2);
      memcpy_s(cameraInfoInstance->selectable, sizeof(bool) * 2, selectable, sizeof(bool) * 2);
   }
   CameraInfoPreset& operator=(GameInternals::CameraInfo* cameraInfoInstance) {
      memcpy_s(stiffness, sizeof(float) * 2, cameraInfoInstance->stiffness, sizeof(float) * 2);
      memcpy_s(angle, sizeof(float) * 2, cameraInfoInstance->angle, sizeof(float) * 2);
      memcpy_s(lag, sizeof(float) * 2, cameraInfoInstance->lag, sizeof(float) * 2);
      memcpy_s(fov, sizeof(float) * 2, cameraInfoInstance->fov, sizeof(float) * 2);
      memcpy_s(height, sizeof(float) * 2, cameraInfoInstance->height, sizeof(float) * 2);
      memcpy_s(lateOffset, sizeof(float) * 2, cameraInfoInstance->lateOffset, sizeof(float) * 2);
      memcpy_s(tilting, sizeof(bool) * 2, cameraInfoInstance->tilting, sizeof(bool) * 2);
      memcpy_s(selectable, sizeof(bool) * 2, cameraInfoInstance->selectable, sizeof(bool) * 2);
      return *this;
   }

   template <class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(stiffness),
         CEREAL_NVP(angle),
         CEREAL_NVP(lag),
         CEREAL_NVP(fov),
         CEREAL_NVP(height),
         CEREAL_NVP(lateOffset),
         CEREAL_NVP(tilting),
         CEREAL_NVP(selectable)
      );
   }
};
// True std::map (de)serialization
namespace cereal {
   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void save(Archive& ar, const std::map<std::string, CameraInfoPreset, C, A>& map) {
      for (const auto& item : map)
         ar(cereal::make_nvp(item.first, item.second));
   }

   template <class Archive, class C, class A,
      traits::EnableIf<traits::is_text_archive<Archive>::value> = traits::sfinae> inline
      void load(Archive& ar, std::map<std::string, CameraInfoPreset, C, A>& map) {
      map.clear();

      auto hint = map.begin();
      while (true) {
         const auto namePtr = ar.getNodeName();
         if (!namePtr)
            break;

         CameraInfoPreset value; ar(value);
         hint = map.emplace_hint(hint, std::move(std::string(namePtr)), std::move(value));
      }
   }
}