#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::CameraInfo;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      class CameraEditor : public _BaseInGameMenuItem {
         std::map<std::string, bool> hasAlreadyLoadedSettings = {};

         int         oldCameraIndex      = 0;
         int*        pActiveCameraIndex  = nullptr;
         bool        showAdvancedOptions = false;

         CameraInfo* pActiveCameraInfo   = nullptr;

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }

         const virtual void onFrame() override {
            pActiveCameraIndex = GameInternals::Gameplay::Camera::getActiveCameraIndexAsPointer();
            if (pActiveCameraIndex) {
               if (oldCameraIndex != *pActiveCameraIndex
                  || ((oldCameraIndex == *pActiveCameraIndex) && (!pActiveCameraInfo))) {
                  if (GameInternals::Gameplay::Camera::getActiveCameraInfo(pActiveCameraInfo)) {
                     std::string activeCameraName = pActiveCameraInfo->CollectionName;
                     auto iter = Settings::settingsType.camPresets.find(activeCameraName);
                     if (iter != Settings::settingsType.camPresets.end()) {
                        if (!hasAlreadyLoadedSettings[activeCameraName]) {
                           auto* activeCameraInfoPreset = &iter->second;
                           activeCameraInfoPreset->loadToCameraInfo(pActiveCameraInfo);

                           hasAlreadyLoadedSettings[activeCameraName] = true;
                        }
                     }
                  }
               }
               oldCameraIndex = *pActiveCameraIndex;
            }
         }

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            return ImGui::Button("Camera Editor", buttonSize);
         }
         const virtual bool displayMenu() override {
            ImGui::PushItemWidth(-1.0f);
            if (pActiveCameraIndex) {
               ImGui::TextWrapped("Camera Index"); ImGui::SameLine();
               if (ImGui::SliderInt("##CameraIndex", pActiveCameraIndex, 0, 6))
                  oldCameraIndex = -1;
               if (pActiveCameraInfo) {
                  ImGui::TextWrapped("Camera Name: %s", pActiveCameraInfo->CollectionName);

                  ImGui::Checkbox("Advanced options", &showAdvancedOptions);
                  ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                  if (ImGui::Button("Save Preset")) {
                     Settings::settingsType.camPresets[std::string(pActiveCameraInfo->CollectionName)] = pActiveCameraInfo;
                     Settings::saveSettings();
                  }

                  if (showAdvancedOptions) {
                     if (ImGui::Button("Copy values to their advanced counterparts")) {
                        pActiveCameraInfo->Stiffness[1]  = pActiveCameraInfo->Stiffness[0];
                        pActiveCameraInfo->Angle[1]      = pActiveCameraInfo->Angle[0];
                        pActiveCameraInfo->Lag[1]        = pActiveCameraInfo->Lag[0];
                        pActiveCameraInfo->FOV[1]        = pActiveCameraInfo->FOV[0];
                        pActiveCameraInfo->Height[1]     = pActiveCameraInfo->Height[0];
                        pActiveCameraInfo->LateOffset[1] = pActiveCameraInfo->LateOffset[0];
                        pActiveCameraInfo->Tilting[1]    = pActiveCameraInfo->Tilting[0];
                        pActiveCameraInfo->Selectable[1] = pActiveCameraInfo->Selectable[0];
                     }
                  }

                  ImGui::TextWrapped("Stiffness");
                  ImGui::SliderFloat("##CameraStiffness", &pActiveCameraInfo->Stiffness[0], 0.0f, 1.0f);
                  if (showAdvancedOptions) {
                     ImGui::Text("STIFFNESS[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraStiffness_1", &pActiveCameraInfo->Stiffness[1], 0.0f, 1.0f);
                  }

                  ImGui::TextWrapped("Horizontal angle");
                  ImGui::SliderFloat("##CameraHorizontalAngle", &pActiveCameraInfo->Angle[0], -45.0f, 45.0f, "%.3f deg");
                  if (showAdvancedOptions) {
                     ImGui::Text("ANGLE[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraHorizontalAngle_1", &pActiveCameraInfo->Angle[1], -45.0f, 45.0f, "%.3f deg");
                  }

                  ImGui::TextWrapped("Distance");
                  ImGui::SliderFloat("##CameraDistance", &pActiveCameraInfo->Lag[0], -100.0f, 100.0f);
                  if (showAdvancedOptions) {
                     ImGui::Text("LAG[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraDistance_1", &pActiveCameraInfo->Lag[1], -100.0f, 100.0f);
                  }

                  ImGui::TextWrapped("FOV");
                  ImGui::SliderFloat("##CameraFOV", &pActiveCameraInfo->FOV[0], 25.0f, 135.0f);
                  if (showAdvancedOptions) {
                     ImGui::Text("FOV[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraFOV_1", &pActiveCameraInfo->FOV[1], 25.0f, 135.0f);
                  }

                  ImGui::TextWrapped("Height");
                  ImGui::SliderFloat("##CameraHeight", &pActiveCameraInfo->Height[0], 0.0f, 100.0f);
                  if (showAdvancedOptions) {
                     ImGui::Text("HEIGHT[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraHeight_1", &pActiveCameraInfo->Height[1], 0.0f, 100.0f);
                  }

                  ImGui::TextWrapped("Vertical angle");
                  ImGui::SliderFloat("##CameraLateOffset", &pActiveCameraInfo->LateOffset[0], -89.0f, 89.0f, "%.3f deg");
                  if (showAdvancedOptions) {
                     ImGui::Text("LATEOFFSET[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraLateOffset_1", &pActiveCameraInfo->LateOffset[1], -89.0f, 89.0f);
                  }

                  ImGui::Checkbox("Tilting", &pActiveCameraInfo->Tilting[0]);
                  if (showAdvancedOptions) {
                     ImGui::Checkbox("TILTING[1]", &pActiveCameraInfo->Tilting[1]);
                  }

                  ImGui::Checkbox("Selectable", &pActiveCameraInfo->Selectable[0]);
                  if (showAdvancedOptions) {
                     ImGui::Checkbox("SELECTABLE[1]", &pActiveCameraInfo->Selectable[1]);
                  }
               } else {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  ImGui::TextWrapped("No active CameraInfo was found.");
                  ImGui::PopStyleColor();
               }
            } else {
               ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
               ImGui::TextWrapped("CameraAI not yet initialized by the game.");
               ImGui::PopStyleColor();
            }
            ImGui::PopItemWidth();
            return false;
         }
      };
   }
}