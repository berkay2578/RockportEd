#pragma once
#include "stdafx.h"
#include "imgui\imgui.h"
// (De)Serialization
#include <cereal\archives\xml.hpp>

struct HUDColoursType {
   ImColor rpmGauge_BackColour                                = ImColor(IM_COL32(0, 0, 0, 0)); // not used
   ImColor rpmGauge_NormalRange_Background_Colour             = ImColor(IM_COL32(150, 150, 150, 100));
   ImColor rpmGauge_NormalRange_Background_Colour_GradientEnd = ImColor(IM_COL32(150, 150, 150, 100));
   ImColor rpmGauge_NormalRange_Foreground_Colour             = ImColor(IM_COL32(255, 255, 255, 200));
   ImColor rpmGauge_NormalRange_Foreground_Colour_GradientEnd = ImColor(IM_COL32(255, 255, 255, 200));
   ImColor rpmGauge_Redline_Background_Colour                 = ImColor(IM_COL32(182, 22, 54, 100));
   ImColor rpmGauge_Redline_Background_Colour_GradientEnd     = ImColor(IM_COL32(182, 22, 54, 100));
   ImColor rpmGauge_Redline_Foreground_Colour                 = ImColor(IM_COL32(217, 26, 64, 200));
   ImColor rpmGauge_Redline_Foreground_Colour_GradientEnd     = ImColor(IM_COL32(217, 26, 64, 200));
   ImColor rpmGauge_Needle_Foreground_Colour                  = ImColor(IM_COL32(255, 255, 255, 255));

   ImColor speedDisplay_FillerText_Colour   = ImColor(ImVec4(0.7f, 0.7f, 0.7f, 0.6f));
   ImColor speedDisplay_ValueText_Colour    = ImColor(ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
   ImColor speedDisplay_UnitTypeText_Colour = ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

   ImColor nosBar_Background_Colour       = ImColor(IM_COL32(0, 0, 0, 0)); // not used
   ImColor nosBar_Value_Background_Colour = ImColor(IM_COL32(22, 27, 10, 255));
   ImColor nosBar_Value_Foreground_Colour = ImColor(IM_COL32(161, 193, 79, 255));

   template <class Archive>
   void serialize(Archive& archive) {
      archive(
         CEREAL_NVP(rpmGauge_BackColour),
         CEREAL_NVP(rpmGauge_NormalRange_Background_Colour),
         CEREAL_NVP(rpmGauge_NormalRange_Background_Colour_GradientEnd),
         CEREAL_NVP(rpmGauge_NormalRange_Foreground_Colour),
         CEREAL_NVP(rpmGauge_NormalRange_Foreground_Colour_GradientEnd),
         CEREAL_NVP(rpmGauge_Redline_Background_Colour),
         CEREAL_NVP(rpmGauge_Redline_Background_Colour_GradientEnd),
         CEREAL_NVP(rpmGauge_Redline_Foreground_Colour),
         CEREAL_NVP(rpmGauge_Redline_Foreground_Colour),
         CEREAL_NVP(rpmGauge_Needle_Foreground_Colour),

         CEREAL_NVP(speedDisplay_FillerText_Colour),
         CEREAL_NVP(speedDisplay_ValueText_Colour),
         CEREAL_NVP(speedDisplay_UnitTypeText_Colour),

         CEREAL_NVP(nosBar_Background_Colour),
         CEREAL_NVP(nosBar_Value_Background_Colour),
         CEREAL_NVP(nosBar_Value_Foreground_Colour)
      );
   }
};

struct SettingsColoursType {
   HUDColoursType hudColours;

   template <class Archive>
   void serialize(Archive& archive) {
      archive(
         cereal::make_nvp("HUD", hudColours)
      );
   }
};