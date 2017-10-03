#pragma once
#include "imgui.h"

namespace ImGui {
   void LoadStyle();
   bool SliderUInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* display_format = "%.0f");
}