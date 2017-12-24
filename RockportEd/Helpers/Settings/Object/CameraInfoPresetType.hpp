#pragma once
#include "Helpers\Game Internals\Gameplay\Camera.h"
// (De)Serialization
#include <cereal\details\traits.hpp>
#include <cereal\types\map.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

struct CameraInfoPreset {
   float Stiffness[2];
   float Angle[2];
   float Lag[2];
   float Fov[2];
   float Height[2];
   float LateOffset[2];
   bool Tilting[2];
   bool Selectable[2];

   void loadToCameraInfo(GameInternals::CameraInfo* cameraInfoInstance) {
      memcpy_s(cameraInfoInstance->Stiffness, sizeof(float) * 2, Stiffness, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->Angle, sizeof(float) * 2, Angle, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->Lag, sizeof(float) * 2, Lag, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->Fov, sizeof(float) * 2, Fov, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->Height, sizeof(float) * 2, Height, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->LateOffset, sizeof(float) * 2, LateOffset, sizeof(float) * 2);
      memcpy_s(cameraInfoInstance->Tilting, sizeof(bool) * 2, Tilting, sizeof(bool) * 2);
      memcpy_s(cameraInfoInstance->Selectable, sizeof(bool) * 2, Selectable, sizeof(bool) * 2);
   }
   CameraInfoPreset& operator=(GameInternals::CameraInfo* cameraInfoInstance) {
      memcpy_s(Stiffness, sizeof(float) * 2, cameraInfoInstance->Stiffness, sizeof(float) * 2);
      memcpy_s(Angle, sizeof(float) * 2, cameraInfoInstance->Angle, sizeof(float) * 2);
      memcpy_s(Lag, sizeof(float) * 2, cameraInfoInstance->Lag, sizeof(float) * 2);
      memcpy_s(Fov, sizeof(float) * 2, cameraInfoInstance->Fov, sizeof(float) * 2);
      memcpy_s(Height, sizeof(float) * 2, cameraInfoInstance->Height, sizeof(float) * 2);
      memcpy_s(LateOffset, sizeof(float) * 2, cameraInfoInstance->LateOffset, sizeof(float) * 2);
      memcpy_s(Tilting, sizeof(bool) * 2, cameraInfoInstance->Tilting, sizeof(bool) * 2);
      memcpy_s(Selectable, sizeof(bool) * 2, cameraInfoInstance->Selectable, sizeof(bool) * 2);
      return *this;
   }

   template <class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(Stiffness),
         CEREAL_NVP(Angle),
         CEREAL_NVP(Lag),
         CEREAL_NVP(Fov),
         CEREAL_NVP(Height),
         CEREAL_NVP(LateOffset),
         CEREAL_NVP(Tilting),
         CEREAL_NVP(Selectable)
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