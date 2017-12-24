#pragma once

namespace ImGui {
   void LoadStyle();

   bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);

   void PushItemDisabled();
   void PopItemDisabled();
}