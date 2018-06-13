#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::CameraInfo;
#include <MMSystem.h> // joy*

namespace Extensions {
   namespace InGameMenu {
      class CameraEditor : public _BaseInGameMenuItem {
         std::map<std::string, bool> hasAlreadyLoadedSettings = {};
         std::map<char*, CameraInfo> backups                  = {};

         bool joyViewEnabled      = false;
         bool speedFOVEnabled     = false;
         bool showAdvancedOptions = false;

         JOYINFOEX               jiEx        = { 0 };
         std::map<char*, float*> joyViewData = {};

         int         oldCameraIndex     = 0;
         int*        pActiveCameraIndex = nullptr;
         CameraInfo* pActiveCameraInfo  = nullptr;

         void resetData() {
            if (pActiveCameraInfo
                && backups.find(pActiveCameraInfo->CollectionName) != backups.end()) {
               auto& bkCameraInfo          = backups[pActiveCameraInfo->CollectionName];
               pActiveCameraInfo->Angle[0] = bkCameraInfo.Angle[0];
               pActiveCameraInfo->Lag[0]   = bkCameraInfo.Lag[0];
               pActiveCameraInfo->FOV[0]   = bkCameraInfo.FOV[0];
            }

            backups.clear();
            if (!joyViewData.empty()) {
               for (auto& allocData : joyViewData)
                  delete[] allocData.second;
               joyViewData.clear();
            }
         }
      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }

         const virtual void onFrame() override {
            pActiveCameraIndex = CameraInternals::getActiveCameraIndexAsPointer();
            if (pActiveCameraIndex) {
               if (oldCameraIndex != *pActiveCameraIndex
                   || ((oldCameraIndex == *pActiveCameraIndex) && (!pActiveCameraInfo))) {
                  if (CameraInternals::getActiveCameraInfo(pActiveCameraInfo)) {
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

            if (joyViewEnabled
                && pActiveCameraInfo
                && joyGetPosEx(JOYSTICKID1, &jiEx) == JOYERR_NOERROR)
            {
               if (backups.find(pActiveCameraInfo->CollectionName) == backups.end())
                  backups[pActiveCameraInfo->CollectionName] = *pActiveCameraInfo;
               if (joyViewData.find(pActiveCameraInfo->CollectionName) == joyViewData.end()) {
                  joyViewData[pActiveCameraInfo->CollectionName] = new float[2] { 5.0f - pActiveCameraInfo->Angle[0],
                                                                                  -2.5f - pActiveCameraInfo->Lag[0] };
               }

               static int32_t edtX, edtY;
               edtX = jiEx.dwUpos - INT16_MAX;
               edtY = jiEx.dwRpos - INT16_MAX;

               static float valX, valY;
               valX = valY = 0.0f;
               if (edtX >> 8)
                  valX = (float)edtX / INT16_MAX;
               if (edtY >> 8)
                  valY = (float)edtY / INT16_MAX;

               static float mod0, mod1;
               mod0 = mod1 = 0.0f;
               if (valX >= 0.1f || valX <= -0.1f) {
                  auto& modifiers = joyViewData[pActiveCameraInfo->CollectionName];
                  mod0 = modifiers[0] * valX;
                  mod1 = modifiers[1] * abs(valX);
               }
               auto& bkCameraInfo = backups[pActiveCameraInfo->CollectionName];
               if (valY >= 0.1f) {
                  mod1 *= -valY;
                  mod1 -= bkCameraInfo.Lag[0] * 2.0f;
               }
               pActiveCameraInfo->Angle[0] = bkCameraInfo.Angle[0] + mod0;
               pActiveCameraInfo->Lag[0]   = bkCameraInfo.Lag[0] + mod1;
            }

            if (speedFOVEnabled
                && pActiveCameraInfo) {
               if (backups.find(pActiveCameraInfo->CollectionName) == backups.end())
                  backups[pActiveCameraInfo->CollectionName] = *pActiveCameraInfo;

               static float playerCarSpeed;
               playerCarSpeed = PlayerCarInternals::getSpeed(GameInternals::SpeedUnit::KMH);

               static float mod0;
               mod0 = 0.0f;
               if (playerCarSpeed > 20.0f)
                  mod0 = (playerCarSpeed / 360.0f) * 30.0f;

               auto& bkCameraInfo = backups[pActiveCameraInfo->CollectionName];
               pActiveCameraInfo->FOV[0] = bkCameraInfo.FOV[0] + mod0;
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
                  if (ImGui::Checkbox("Controller View", &joyViewEnabled)) {
                     if (joyViewEnabled) {
                        jiEx.dwSize = sizeof(JOYINFOEX);
                        joySetCapture(Helpers::WndProcHook::windowHandle, JOYSTICKID1, 1, FALSE);
                     } else {
                        joyReleaseCapture(JOYSTICKID1);
                        resetData();
                     }
                  } ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                  if (ImGui::Checkbox("Speed FOV", &speedFOVEnabled)) {
                     if (!speedFOVEnabled)
                        resetData();
                  }

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

                  if (!joyViewEnabled) {
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
                  } else {
                     ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                     ImGui::TextWrapped("Cannot change horizontal angle or distance while 'Controller View' is enabled!");
                     ImGui::PopStyleColor();
                  }

                  if (!speedFOVEnabled) {
                     ImGui::TextWrapped("FOV");
                     ImGui::SliderFloat("##CameraFOV", &pActiveCameraInfo->FOV[0], 25.0f, 135.0f);
                     if (showAdvancedOptions) {
                        ImGui::Text("FOV[1]"); ImGui::SameLine();
                        ImGui::SliderFloat("##CameraFOV_1", &pActiveCameraInfo->FOV[1], 25.0f, 135.0f);
                     }
                  } else {
                     ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                     ImGui::TextWrapped("Cannot change FOV while 'Speed FOV' is enabled!");
                     ImGui::PopStyleColor();
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
                  resetData();
               }
            } else {
               ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
               ImGui::TextWrapped("CameraAI not yet initialized by the game.");
               ImGui::PopStyleColor();
               resetData();
            }
            ImGui::PopItemWidth();
            return true;
         }
      };
   }
}