#pragma once
// InGameMenu
#include <d3d9.h>
#include "InGame Menu\Items\_BaseInGameMenuItem.hpp"
// DirectInput8
#include <dinput.h>
// Helpers
#include "Helpers\Settings\Settings.h"
#include "Helpers\Game Internals\GameInternals.h"
#include MIRRORHOOK_DEFINITIONS_PATH
// ImGui
#include "Helpers\imgui\imgui.h"
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   extern ImGuiIO* imguiIO; // defined in InGameMenu.cpp

   namespace RockportEdControls {
      typedef std::function<bool(const char*)> CreatePresetCallback;
      typedef std::function<bool(LPVOID*, const char**)> LoadPresetCallback;
      typedef std::function<bool(void)> UpdateActivePresetCallback;
      typedef std::function<bool(LPVOID*, const char**)> DeletePresetCallback;
      typedef std::function<bool(LPVOID*, const char**)> ListPresetsFn;

      class PresetControls {
         bool hasLoadedAPreset = false;

         LPVOID*      ppActivePreset     = nullptr;
         const char** ppActivePresetName = nullptr;

         LPVOID      pListActivePreset     = nullptr;
         const char* pListActivePresetName = nullptr;

         CreatePresetCallback       createPresetCallback       = nullptr;
         LoadPresetCallback         loadPresetCallback         = nullptr;
         UpdateActivePresetCallback updateActivePresetCallback = nullptr;
         DeletePresetCallback       deletePresetCallback       = nullptr;
         ListPresetsFn              listPresetsFn              = nullptr;

      public:
         PresetControls() = default;
         void Set(
            const CreatePresetCallback& in_createPresetCallback, const LoadPresetCallback& in_loadPresetCallBack,
            const UpdateActivePresetCallback& in_updateActivePresetCallback, const DeletePresetCallback& in_deletePresetCallback,
            const ListPresetsFn& in_listPresetsFn,
            LPVOID* in_ppActivePresetVariable, const char** in_ppActivePresetNameVariable)
         {
            ppActivePreset     = in_ppActivePresetVariable;
            ppActivePresetName = in_ppActivePresetNameVariable;

            createPresetCallback       = in_createPresetCallback;
            loadPresetCallback         = in_loadPresetCallBack;
            updateActivePresetCallback = in_updateActivePresetCallback;
            deletePresetCallback       = in_deletePresetCallback;
            listPresetsFn              = in_listPresetsFn;
         }

         void Draw() {
            if (ImGui::BeginChild("##Presets", ImVec2(0.0f, 92.0f), true))
            {
               if (ImGui::Button("Create preset"))
                  ImGui::OpenPopup("##CreatePresetPopup");
               if (ImGui::BeginPopup("##CreatePresetPopup", ImGuiWindowFlags_AlwaysAutoResize)) {
                  static char presetName[64] = { 0 };
                  ImGui::Text("Preset name: "); ImGui::SameLine();
                  ImGui::InputText("##PresetName", presetName, sizeof(presetName), ImGuiInputTextFlags_CallbackCharFilter, ImGui::ExAlphaNumericFilter);

                  ImGui::SameLine();
                  if (ImGui::Button("Save")) {
                     if (createPresetCallback(presetName)) {
                        hasLoadedAPreset = true;
                     } else {
                        *ppActivePreset     = nullptr;
                        *ppActivePresetName = nullptr;
                        hasLoadedAPreset    = false;
                     }
                     ZeroMemory(presetName, sizeof(presetName));
                     ImGui::CloseCurrentPopup();
                  }

                  ImGui::EndPopup();
               }
               if (hasLoadedAPreset && *ppActivePreset && *ppActivePresetName) {
                  ImGui::SameLine(); ImGui::VerticalSeparator();  ImGui::SameLine();
                  static char buttonText[128] = { 0 };
                  if (snprintf(buttonText, sizeof(buttonText), "Update '%s'", *ppActivePresetName)) {
                     if (ImGui::Button(buttonText)) {
                        if (updateActivePresetCallback())
                           Settings::saveSettings();
                     }
                  }
               }
               ImGui::Separator();
               ImGui::Text("Presets: "); ImGui::SameLine();
               if (listPresetsFn(&pListActivePreset, &pListActivePresetName)) {
                  if (pListActivePreset && pListActivePresetName) {
                     ImGui::Text("Actions: "); ImGui::SameLine();
                     if (ImGui::Button("Load")) {
                        if (loadPresetCallback(&pListActivePreset, &pListActivePresetName)) {
                           *ppActivePreset     = pListActivePreset;
                           *ppActivePresetName = pListActivePresetName;
                           hasLoadedAPreset    = true;
                        }
                     } ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                     if (ImGui::Button("Delete")) {
                        if (deletePresetCallback(&pListActivePreset, &pListActivePresetName)) {
                           pListActivePreset     = *ppActivePreset;
                           pListActivePresetName = *ppActivePresetName;
                        } else {
                           pListActivePreset     = nullptr;
                           pListActivePresetName = nullptr;
                           hasLoadedAPreset      = false;
                        }
                     }
                  }
               } else {
                  ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "None.");
               }
            }
            ImGui::EndChild();
            ImGui::Separator();
         }
      };
   }
}