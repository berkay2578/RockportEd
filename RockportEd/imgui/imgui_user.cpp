#include "imgui_user.h"

namespace ImGui {
   void LoadStyle() {
      ImGuiStyle* style = &ImGui::GetStyle();

      style->WindowMinSize       = ImVec2(10.0f, 10.0f);
      style->WindowPadding       = ImVec2(10.0f, 10.0f);
      style->WindowRounding      = 5.0f;
      style->ChildWindowRounding = 5.0f;
      style->FramePadding        = ImVec2(5.0f, 4.0f);
      style->FrameRounding       = 5.0f;
      style->ItemSpacing         = ImVec2(5.0f, 5.0f);
      style->ItemInnerSpacing    = ImVec2(10.0f, 10.0f);
      style->IndentSpacing       = 15.0f;
      style->ScrollbarSize       = 16.0f;
      style->ScrollbarRounding   = 5.0f;
      style->GrabMinSize         = 7.0f;
      style->GrabRounding        = 2.0f;

      style->Colors[ImGuiCol_Text]                 = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
      style->Colors[ImGuiCol_WindowBg]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_ChildWindowBg]        = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
      style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);
      style->Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
      style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      style->Colors[ImGuiCol_FrameBg]              = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
      style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
      style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
      style->Colors[ImGuiCol_TitleBg]              = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
      style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.78f, 0.78f, 0.78f, 0.39f);
      style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
      style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
      style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
      style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
      style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
      style->Colors[ImGuiCol_ComboBg]              = ImVec4(0.78f, 0.78f, 0.78f, 0.98f);
      style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
      style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
      style->Colors[ImGuiCol_Button]               = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
      style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
      style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
      style->Colors[ImGuiCol_Header]               = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
      style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.20f, 0.51f, 0.67f, 1.00f);
      style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
      style->Colors[ImGuiCol_Separator]            = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
      style->Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
      style->Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.27f, 0.59f, 0.75f, 0.78f);
      style->Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.08f, 0.39f, 0.55f, 0.78f);
      style->Colors[ImGuiCol_CloseButton]          = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
      style->Colors[ImGuiCol_CloseButtonHovered]   = ImVec4(0.71f, 0.71f, 0.71f, 0.60f);
      style->Colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
      style->Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
   }
   bool SliderUInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* display_format) {
      if (!display_format)
         display_format = "%.0f";
      float v_f = (float)*v;
      bool value_changed = SliderFloat(label, &v_f, (float)v_min, (float)v_max, display_format, 1.0f);
      *v = (int)v_f;
      return value_changed;
   }
}