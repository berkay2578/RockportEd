#include "imgui.h"
#include "imgui_internal.h"
#include <vector>

namespace ImGui {
   void LoadStyle() {
      ImGuiStyle* style = &ImGui::GetStyle();

      style->AntiAliasedLines  = true;
      style->AntiAliasedFill   = true;
      style->Alpha             = 0.9f;

      style->ButtonTextAlign   = ImVec2(0.0f, 0.5f);
      style->WindowTitleAlign  = ImVec2(0.0f, 0.5f);
      style->WindowMinSize     = ImVec2(150.0f, 100.0f);

      style->ChildBorderSize  = 0.0f;
      style->FrameBorderSize  = 0.0f;
      style->PopupBorderSize  = 0.4f;
      style->WindowBorderSize = 0.0f;

      style->WindowPadding       = ImVec2(10.0f, 10.0f);
      style->WindowRounding      = 2.0f;
      style->ChildRounding       = 1.0f;
      style->FramePadding        = ImVec2(4.0f, 3.0f);
      style->FrameRounding       = 0.0f;
      style->ItemSpacing         = ImVec2(8.0f, 4.0f);
      style->ItemInnerSpacing    = ImVec2(4.0f, 4.0f);
      style->IndentSpacing       = 25.0f;
      style->ScrollbarSize       = 16.0f;
      style->ScrollbarRounding   = 9.0f;
      style->GrabMinSize         = 10.0f;
      style->GrabRounding        = 0.0f;

      style->Colors[ImGuiCol_Text]                 = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
      style->Colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.06f, 0.06f, 0.70f);
      style->Colors[ImGuiCol_ChildBg]              = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
      style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.14f, 0.14f, 0.14f, 0.99f);
      style->Colors[ImGuiCol_Border]               = ImVec4(1.00f, 1.00f, 1.00f, 0.39f);
      style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);
      style->Colors[ImGuiCol_FrameBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
      style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
      style->Colors[ImGuiCol_TitleBg]              = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
      style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
      style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
      style->Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
      style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
      style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.31f, 0.31f, 0.31f, 0.80f);
      style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.51f, 0.51f, 0.51f, 0.80f);
      style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
      style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
      style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
      style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
      style->Colors[ImGuiCol_Button]               = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
      style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
      style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
      style->Colors[ImGuiCol_Header]               = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
      style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
      style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
      style->Colors[ImGuiCol_Separator]            = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
      style->Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
      style->Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
      style->Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      style->Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
      style->Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
      style->Colors[ImGuiCol_CloseButton]          = ImVec4(0.41f, 0.41f, 0.41f, 0.50f);
      style->Colors[ImGuiCol_CloseButtonHovered]   = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
      style->Colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
      style->Colors[ImGuiCol_PlotLines]            = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
      style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
      style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
   }

   bool customGetter_Vector(void* vec, int idx, const char** out_text) {
      auto& vector = *static_cast<std::vector<std::string>*>(vec);
      if (idx < 0 || idx >= static_cast<int>(vector.size())) {
         return false;
      }
      *out_text = vector.at(idx).c_str();
      return true;
   };
   bool Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
      if (values.empty()) {
         return false;
      }
      return Combo(label, currIndex, customGetter_Vector, static_cast<void*>(&values), values.size());
   }

   void PushItemDisabled() {
      PushItemFlag(ImGuiItemFlags_Disabled, true);
      PushStyleVar(ImGuiStyleVar_Alpha, GetStyle().Alpha * 0.5f);
   }
   void PopItemDisabled() {
      ImGui::PopItemFlag();
      ImGui::PopStyleVar();
   }
}