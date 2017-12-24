#pragma once
#include "imgui.h"
#include "imgui_internal.h"

inline ImVec2 operator-(const ImVec2& l, const ImVec2& r) {
   return{ l.x - r.x, l.y - r.y };
}
inline ImVec2 operator+(const ImVec2& l, const ImVec2& r) {
   return{ l.x + r.x, l.y + r.y };
}
inline ImVec2 operator/(const ImVec2& vec, const size_t div) {
   return{ vec.x / div, vec.y / div };
}

int rotation_start_index;
void ImRotateStart() {
   rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImRotationCenter() {
   ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

   const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
   for (int i = rotation_start_index; i < buf.Size; i++)
      l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

   return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}

void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter()) {
   float c=sin(rad), s=cos(rad); // reversed to suit with ImDrawList->PathArcTo angle calculation
   center = ImRotate(center, s, c) - center;

   auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
   for (int i = rotation_start_index; i < buf.Size; i++)
      buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}