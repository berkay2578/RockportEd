#pragma once
#include "stdafx.h"
#include "imgui\imgui.h"

namespace ImGui {
   bool SliderUInt(const char* label, UINT* v, UINT v_min, UINT v_max, const char* display_format = "%.0f");
}