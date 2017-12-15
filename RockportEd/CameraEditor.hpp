#pragma once
#include "stdafx.h"
#include "Extensions.h"
using GameInternals::CameraInfo;
// ImGui::VerticalSeparator
#include "imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      struct CameraEditor : _BaseInGameMenuItem {
      private:
         std::map<std::string, bool> hasAlreadyLoadedSettings = {};

         int         oldCameraIndex      = 0;
         int*        activeCameraIndex   = nullptr;
         bool        showAdvancedOptions = false;

         CameraInfo* activeCameraInfo = nullptr;

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }
         const virtual void displayMenu() override {
            ImGui::Checkbox("Camera Editor", &isEnabled);
            if (isEnabled) {
               ImGui::SetNextWindowSize(ImVec2(240.0f, 400.0f), ImGuiCond_Once);
               ImGui::SetNextWindowPos(ImVec2(10.0f * 2 + 200.0f, 5.0f), ImGuiCond_Once);
               if (ImGui::Begin("Camera Editor", &isEnabled)) {
                  ImGui::PushItemWidth(-1.0f);

                  activeCameraIndex = GameInternals::Gameplay::Camera::getActiveCameraIndexAsPointer();
                  if (activeCameraIndex) {
                     ImGui::TextWrapped("Camera Index"); ImGui::SameLine();
                     ImGui::SliderInt("##CameraIndex", activeCameraIndex, 0, 6);

                     if (GameInternals::Gameplay::Camera::getActiveCameraInfo(activeCameraInfo)) {
                        if (oldCameraIndex != *activeCameraIndex) {
                           std::string activeCameraName = activeCameraInfo->collectionName;
                           auto iter = Settings::settingsType.camPresets.find(activeCameraName);
                           if (iter != Settings::settingsType.camPresets.end()) {
                              if (!hasAlreadyLoadedSettings[activeCameraName]) {
                                 auto* activeCameraInfoPreset = &iter->second;
                                 activeCameraInfoPreset->loadToCameraInfo(activeCameraInfo);

                                 hasAlreadyLoadedSettings[activeCameraName] = true;
                              }
                           }
                        }
                        ImGui::TextWrapped("Camera Name: %s", activeCameraInfo->collectionName);

                        ImGui::Checkbox("Advanced options", &showAdvancedOptions);
                        ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                        if (ImGui::Button("Save Preset")) {
                           Settings::settingsType.camPresets[std::string(activeCameraInfo->collectionName)] = activeCameraInfo;
                           Settings::saveSettings();
                        }

                        ImGui::TextWrapped("Stiffness");
                        ImGui::SliderFloat("##CameraStiffness", &activeCameraInfo->Stiffness[0], 0.0f, 1.0f);
                        if (showAdvancedOptions) {
                           ImGui::Text("STIFFNESS[1]"); ImGui::SameLine();
                           ImGui::SliderFloat("##CameraStiffness_1", &activeCameraInfo->Stiffness[1], 0.0f, 1.0f);
                        }

                        ImGui::TextWrapped("Horizontal angle");
                        ImGui::SliderFloat("##CameraHorizontalAngle", &activeCameraInfo->Angle[0], -45.0f, 45.0f, "%.3f deg");
                        if (showAdvancedOptions) {
                           ImGui::Text("ANGLE[1]"); ImGui::SameLine();
                           ImGui::SliderFloat("##CameraHorizontalAngle_1", &activeCameraInfo->Angle[1], -45.0f, 45.0f, "%.3f deg");
                        }

                        ImGui::TextWrapped("Distance");
                        ImGui::SliderFloat("##CameraDistance", &activeCameraInfo->Lag[0], -100.0f, 100.0f);
                        if (showAdvancedOptions) {
                           ImGui::Text("LAG[1]"); ImGui::SameLine();
                           ImGui::SliderFloat("##CameraDistance_1", &activeCameraInfo->Lag[1], -100.0f, 100.0f);
                        }

                        ImGui::TextWrapped("FOV");
                        ImGui::SliderFloat("##CameraFOV", &activeCameraInfo->Fov[0], 25.0f, 135.0f);
                        if (showAdvancedOptions) {
                           ImGui::Text("FOV[1]"); ImGui::SameLine();
                           ImGui::SliderFloat("##CameraFOV_1", &activeCameraInfo->Fov[1], 25.0f, 135.0f);
                        }

                        ImGui::TextWrapped("Height");
                        ImGui::SliderFloat("##CameraHeight", &activeCameraInfo->Height[0], 0.0f, 100.0f);
                        if (showAdvancedOptions) {
                           ImGui::Text("HEIGHT[1]"); ImGui::SameLine();
                           ImGui::SliderFloat("##CameraHeight_1", &activeCameraInfo->Height[1], 0.0f, 100.0f);
                        }

                        ImGui::TextWrapped("Vertical angle");
                        ImGui::SliderFloat("##CameraLateOffset", &activeCameraInfo->LateOffset[0], -89.0f, 89.0f, "%.3f deg");
                        if (showAdvancedOptions) {
                           ImGui::Text("LATEOFFSET[1]"); ImGui::SameLine();
                           ImGui::SliderFloat("##CameraLateOffset_1", &activeCameraInfo->LateOffset[1], -89.0f, 89.0f);
                        }

                        ImGui::Checkbox("Tilting", &activeCameraInfo->Tilting[0]);
                        if (showAdvancedOptions) {
                           ImGui::Checkbox("TILTING[1]", &activeCameraInfo->Tilting[1]);
                        }

                        ImGui::Checkbox("Selectable", &activeCameraInfo->Selectable[0]);
                        if (showAdvancedOptions) {
                           ImGui::Checkbox("SELECTABLE[1]", &activeCameraInfo->Selectable[1]);
                        }
                     }
                     else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        ImGui::TextWrapped("CameraInfo not yet initialized by the game.");
                        ImGui::PopStyleColor();
                     }
                     oldCameraIndex = *activeCameraIndex;
                  }
                  else {
                     ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                     ImGui::TextWrapped("CameraAI not yet initialized by the game.");
                     ImGui::PopStyleColor();
                     oldCameraIndex = 0;
                  }
                  ImGui::PopItemWidth();
               }
               ImGui::End();
            }
         }
      };
   }
}