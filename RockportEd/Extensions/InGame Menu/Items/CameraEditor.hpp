/*
   MIT License
   
   Copyright (c) Berkay Yigit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::CameraInfo;
#include <MMSystem.h> // joy*

namespace Extensions {
   namespace InGameMenu {
      class CameraEditor : public _BaseInGameMenuItem {
         struct CameraEditorData {
            CameraInfo defaultCameraInfo          = { 0 };
            bool       showAdvancedOptions        = false;
            bool       hasLoadedSettings          = false;
            bool       hasLoadedDefaultCameraInfo = false;

            bool  joyViewEnabled       = false;
            float joyViewData[2]       = { 0.0f, 0.0f };
            bool  hasLoadedJoyViewData = false;

            void enableForceLookBack(const bool& shouldForce) {
               if (shouldForce)
                  Memory::writeRaw(0x6B6890, true, 4, 0x83, 0xF0, 0x01, 0xC3); // xor eax,1; ret;
               else
                  Memory::writeRaw(0x6B6890, true, 4, 0xD9, 0x81, 0xDC, 0x02); // fld [ecx+0x2DC0];
            }

            bool  speedFOVEnabled = false;
            float speedFOVScale   = 20.0f;

            int16_t nitrousFOVWidening = 1638; // Game default value, 0x666
            float   nitrousFOVScale    = 5.0f; // nitrousFOVWidening / 327.6

            float* getNitrousFOVWideningAsPScale() {
               nitrousFOVScale = nitrousFOVWidening / 327.6f;
               return &nitrousFOVScale;
            }
            void setNitrousFOVScaleToWidening(const bool& writeToMemory = true) {
               nitrousFOVWidening = (int16_t)(nitrousFOVScale * 327.6f);
               if (writeToMemory)
                  InternalVariables::setVariable(InternalVariables::nosFOVWidening, nitrousFOVWidening);
            }
         };
         std::map<CameraInfo*, CameraEditorData> cache = {};
         CameraEditorData*     pActiveCameraEditorData = nullptr;

         int         oldCameraIndex     = 0;
         int*        pActiveCameraIndex = nullptr;
         CameraInfo* pActiveCameraInfo  = nullptr;

         void resetData(const bool& resetJoyView, const bool& resetSpeedFOV) {
            if (!pActiveCameraInfo)
               return;

            CameraEditorData* pCameraEditorData = pActiveCameraEditorData;
            if (!pCameraEditorData) {
               auto iter = cache.find(pActiveCameraInfo);
               if (iter != cache.end()) {
                  pCameraEditorData = &iter->second;
               }
            }
            if (pCameraEditorData) {
               if (resetJoyView) {
                  pActiveCameraInfo->Angle[0] = pCameraEditorData->defaultCameraInfo.Angle[0];
                  pActiveCameraInfo->Lag[0]   = pCameraEditorData->defaultCameraInfo.Lag[0];
                  pActiveCameraEditorData->enableForceLookBack(false);
                  pActiveCameraEditorData->hasLoadedJoyViewData = false;
               }
               if (resetSpeedFOV)
                  pActiveCameraInfo->FOV[0] = pCameraEditorData->defaultCameraInfo.FOV[0];
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
                     pActiveCameraEditorData = &cache[pActiveCameraInfo];

                     if (!pActiveCameraEditorData->hasLoadedSettings) {
                        std::string activeCameraName = pActiveCameraInfo->CollectionName;
                        auto iter = Settings::settingsType.cameraPresets.find(activeCameraName);
                        if (iter != Settings::settingsType.cameraPresets.end()) {
                           auto* pActiveCameraPreset = &iter->second;

                           *pActiveCameraInfo                          = pActiveCameraPreset->InfoPreset.getGameInternalsCompliantData();
                           pActiveCameraEditorData->joyViewEnabled     = pActiveCameraPreset->JoyViewEnabled;
                           pActiveCameraEditorData->speedFOVEnabled    = pActiveCameraPreset->SpeedFOVEnabled;
                           pActiveCameraEditorData->speedFOVScale      = pActiveCameraPreset->SpeedFOVScale;
                           pActiveCameraEditorData->nitrousFOVWidening = pActiveCameraPreset->NitrousFOVWidening;

                           pActiveCameraEditorData->hasLoadedSettings = true;
                        }
                     }
                     pActiveCameraEditorData->defaultCameraInfo = *pActiveCameraInfo;
                     InternalVariables::setVariable(InternalVariables::nosFOVWidening, pActiveCameraEditorData->nitrousFOVWidening);
                  }
               } oldCameraIndex = *pActiveCameraIndex;

               if (pActiveCameraInfo)
                  pActiveCameraEditorData = &cache[pActiveCameraInfo];
               if (pActiveCameraInfo && pActiveCameraEditorData) {
                  if (pActiveCameraEditorData->joyViewEnabled) {
                     if (!pActiveCameraEditorData->hasLoadedJoyViewData) {
                        pActiveCameraEditorData->joyViewData[0]       = 5.0f - pActiveCameraInfo->Angle[0];
                        pActiveCameraEditorData->joyViewData[1]       = -pActiveCameraInfo->Lag[0];
                        pActiveCameraEditorData->hasLoadedJoyViewData = true;
                     }

                     static JOYINFOEX jiEx = { 0 };
                     jiEx.dwFlags = JOY_RETURNU | JOY_RETURNR | JOY_RETURNBUTTONS;
                     jiEx.dwSize  = sizeof(JOYINFOEX);

                     if (joyGetPosEx(JOYSTICKID1, &jiEx) == JOYERR_NOERROR)
                     {
                        static int32_t edtX, edtY;
                        edtX = jiEx.dwUpos - INT16_MAX;
                        edtY = jiEx.dwRpos - INT16_MAX;

                        static float valX, valY;
                        valX = valY = 0.0f;
                        if (edtX >> 4)
                           valX = (float)edtX / INT16_MAX;
                        if (edtY >> 4)
                           valY = (float)edtY / INT16_MAX;

                        auto& modifiers = pActiveCameraEditorData->joyViewData;
                        static float mod0, mod1;
                        mod0 = mod1 = 0.0f;
                        if (valX >= 0.05f || valX <= -0.05f) {
                           mod0 = modifiers[0] * valX;
                           mod1 = modifiers[1] * abs(valX);
                        }
                        if (valY >= 0.05f) {
                           mod1 -= modifiers[1] * (valY * -1.8f);
                        }
                        auto& bkCameraInfo = pActiveCameraEditorData->defaultCameraInfo;
                        pActiveCameraInfo->Angle[0] = bkCameraInfo.Angle[0] + mod0;
                        pActiveCameraInfo->Lag[0]   = bkCameraInfo.Lag[0] + mod1;

                        if (jiEx.dwButtons & JOY_BUTTON10) // xbox 360 right stick down
                           pActiveCameraEditorData->enableForceLookBack(true);
                        else
                           pActiveCameraEditorData->enableForceLookBack(false);
                     }
                  }

                  if (pActiveCameraEditorData->speedFOVEnabled) {
                     static float playerCarSpeed;
                     playerCarSpeed = PlayerCarInternals::getSpeed(GameInternals::SpeedUnit::KMH);

                     static float mod0;
                     mod0 = 0.0f;
                     if (playerCarSpeed > 10.0f)
                        mod0 = min(pActiveCameraEditorData->speedFOVScale, (playerCarSpeed / 360.0f) * pActiveCameraEditorData->speedFOVScale);

                     pActiveCameraInfo->FOV[0] = pActiveCameraEditorData->defaultCameraInfo.FOV[0] + mod0;
                  }
               }
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
                  if (ImGui::Checkbox("Controller View", &pActiveCameraEditorData->joyViewEnabled)) {
                     if (pActiveCameraEditorData->joyViewEnabled) {
                        if (!pActiveCameraEditorData->hasLoadedDefaultCameraInfo) {
                           pActiveCameraEditorData->defaultCameraInfo          = *pActiveCameraInfo;
                           pActiveCameraEditorData->hasLoadedDefaultCameraInfo = true;
                        } else {
                           memcpy_s(pActiveCameraEditorData->defaultCameraInfo.Angle, sizeof(float) * 2, pActiveCameraInfo->Angle, sizeof(float) * 2);
                           memcpy_s(pActiveCameraEditorData->defaultCameraInfo.Lag, sizeof(float) * 2, pActiveCameraInfo->Lag, sizeof(float) * 2);
                        }

                        joySetCapture(Helpers::WndProcHook::windowHandle, JOYSTICKID1, 1, FALSE);
                     } else {
                        joyReleaseCapture(JOYSTICKID1);
                        resetData(true, false);
                     }
                  } ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                  if (ImGui::Checkbox("Speed FOV", &pActiveCameraEditorData->speedFOVEnabled)) {
                     if (pActiveCameraEditorData->speedFOVEnabled) {
                        if (!pActiveCameraEditorData->hasLoadedDefaultCameraInfo) {
                           pActiveCameraEditorData->defaultCameraInfo          = *pActiveCameraInfo;
                           pActiveCameraEditorData->hasLoadedDefaultCameraInfo = true;
                        } else {
                           memcpy_s(pActiveCameraEditorData->defaultCameraInfo.FOV, sizeof(float) * 2, pActiveCameraInfo->FOV, sizeof(float) * 2);
                        }
                     } else
                        resetData(false, true);
                  }
                  if (pActiveCameraEditorData->speedFOVEnabled)
                     ImGui::SliderFloat("##SpeedFOVScale", &pActiveCameraEditorData->speedFOVScale, -50.0f, 50.0f, "Speed FOV Scale: %.1f");

                  if (ImGui::SliderFloat("##NitrousFOVScale", pActiveCameraEditorData->getNitrousFOVWideningAsPScale(), -30.0f, 30.0f, "NOS FOV Scale: %.1f"))
                     pActiveCameraEditorData->setNitrousFOVScaleToWidening();

                  ImGui::Checkbox("Advanced options", &pActiveCameraEditorData->showAdvancedOptions);
                  ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                  if (ImGui::Button("Save Preset")) {
                     auto* pCameraPreset = &Settings::settingsType.cameraPresets[std::string(pActiveCameraInfo->CollectionName)];
                     pCameraPreset->InfoPreset.setTo(pActiveCameraInfo);
                     pCameraPreset->JoyViewEnabled     = pActiveCameraEditorData->joyViewEnabled;
                     pCameraPreset->SpeedFOVEnabled    = pActiveCameraEditorData->speedFOVEnabled;
                     pCameraPreset->SpeedFOVScale      = pActiveCameraEditorData->speedFOVScale;
                     pCameraPreset->NitrousFOVWidening = InternalVariables::getVariable(InternalVariables::nosFOVWidening);

                     Settings::saveSettings();
                  }

                  if (pActiveCameraEditorData->showAdvancedOptions) {
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
                  if (pActiveCameraEditorData->showAdvancedOptions) {
                     ImGui::Text("STIFFNESS[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraStiffness_1", &pActiveCameraInfo->Stiffness[1], 0.0f, 1.0f);
                  }

                  if (!pActiveCameraEditorData->joyViewEnabled) {
                     ImGui::TextWrapped("Horizontal angle");
                     ImGui::SliderFloat("##CameraHorizontalAngle", &pActiveCameraInfo->Angle[0], -45.0f, 45.0f, "%.3f deg");
                     if (pActiveCameraEditorData->showAdvancedOptions) {
                        ImGui::Text("ANGLE[1]"); ImGui::SameLine();
                        ImGui::SliderFloat("##CameraHorizontalAngle_1", &pActiveCameraInfo->Angle[1], -45.0f, 45.0f, "%.3f deg");
                     }

                     ImGui::TextWrapped("Distance");
                     ImGui::SliderFloat("##CameraDistance", &pActiveCameraInfo->Lag[0], -100.0f, 100.0f);
                     if (pActiveCameraEditorData->showAdvancedOptions) {
                        ImGui::Text("LAG[1]"); ImGui::SameLine();
                        ImGui::SliderFloat("##CameraDistance_1", &pActiveCameraInfo->Lag[1], -100.0f, 100.0f);
                     }
                  } else {
                     ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                     ImGui::TextWrapped("Cannot change horizontal angle or distance while 'Controller View' is enabled!");
                     ImGui::PopStyleColor();
                  }

                  if (!pActiveCameraEditorData->speedFOVEnabled) {
                     ImGui::TextWrapped("FOV");
                     ImGui::SliderFloat("##CameraFOV", &pActiveCameraInfo->FOV[0], 25.0f, 135.0f);
                     if (pActiveCameraEditorData->showAdvancedOptions) {
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
                  if (pActiveCameraEditorData->showAdvancedOptions) {
                     ImGui::Text("HEIGHT[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraHeight_1", &pActiveCameraInfo->Height[1], 0.0f, 100.0f);
                  }

                  ImGui::TextWrapped("Vertical angle");
                  ImGui::SliderFloat("##CameraLateOffset", &pActiveCameraInfo->LateOffset[0], -89.0f, 89.0f, "%.3f deg");
                  if (pActiveCameraEditorData->showAdvancedOptions) {
                     ImGui::Text("LATEOFFSET[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraLateOffset_1", &pActiveCameraInfo->LateOffset[1], -89.0f, 89.0f);
                  }

                  ImGui::Checkbox("Tilting", &pActiveCameraInfo->Tilting[0]);
                  if (pActiveCameraEditorData->showAdvancedOptions) {
                     ImGui::Checkbox("TILTING[1]", &pActiveCameraInfo->Tilting[1]);
                  }

                  ImGui::Checkbox("Selectable", &pActiveCameraInfo->Selectable[0]);
                  if (pActiveCameraEditorData->showAdvancedOptions) {
                     ImGui::Checkbox("SELECTABLE[1]", &pActiveCameraInfo->Selectable[1]);
                  }
               } else {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  ImGui::TextWrapped("No active CameraInfo was found.");
                  ImGui::PopStyleColor();
                  resetData(true, true);
               }
            } else {
               ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
               ImGui::TextWrapped("CameraAI not yet initialized by the game.");
               ImGui::PopStyleColor();
               resetData(true, true);
            }
            ImGui::PopItemWidth();
            return true;
         }
      };
   }
}