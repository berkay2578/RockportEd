#pragma once
#include <vector>
#include "Helpers\imgui\imgui.h"

namespace ImGui {   
   void LoadStyle();

   bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);

   void PushItemDisabled();
   void PopItemDisabled();

   void   ImRotateStart();
   ImVec2 ImRotationCenter();
   void   ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());

   int ExAlphaNumericFilter(ImGuiTextEditCallbackData* data);
}