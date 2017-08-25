#include "stdafx.h"
#include "imguiEx.h"
#include "imgui\imgui_internal.h"

namespace ImGui {
   ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }

   bool SliderUInt(const char* label, UINT* v, UINT v_min, UINT v_max, const char* display_format) {
      if (!display_format)
         display_format = "%.0f";
      float v_f = (float)*v;
      bool value_changed = SliderFloat(label, &v_f, (float)v_min, (float)v_max, display_format, 1.0f);
      *v = (int)v_f;
      return value_changed;
   }
}