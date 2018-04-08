#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::TimeOfDay;
using GameInternals::TimeOfDayLighting;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      class TimeOfDayEditor : public _BaseInGameMenuItem {
         DWORD* curLighting_pt1 = nullptr;
         DWORD* curLighting_pt2 = nullptr;

         std::vector<std::string> lightingHashes              = std::vector<std::string>();
         std::map<int, DWORD>     lightingDefinitionAddresses = {};
         int                      currentLightingIndex        = 0;

         TimeOfDay*               todInstance                 = nullptr;
         TimeOfDayLighting*       todLightingInstance         = nullptr;
         TimeOfDayLightingPreset* todLightingSettingsInstance = nullptr;

      public:
         const virtual void loadData() override {
            const DWORD* todLightingsArray = nullptr;
            while (!todLightingsArray) {
               todLightingsArray = Memory::readPointer(0x5B392C, 3, 0xEC, 0x14, 0x1C);
               Sleep(10);
            }

            int step = 0;
            int lightingIndex = 0;
            while (true) {
               DWORD hash = *(DWORD*)(*todLightingsArray + step);
               if (hash == 0xAAAAAAAA) {
                  break;
               } else if (hash) {
                  char* strHash = new char[10 + 1];
                  sprintf_s(strHash, 10 + 1, "0x%08X", hash);
                  std::string strHash_proper(strHash);
                  lightingHashes.push_back(strHash_proper);
                  delete[] strHash;

                  lightingDefinitionAddresses[lightingIndex] = *(DWORD*)(*todLightingsArray + step + 0x4);
                  if (Settings::settingsType.todPresets.size() != 0) {
                     auto iter = Settings::settingsType.todPresets.find(strHash_proper);
                     if (iter != Settings::settingsType.todPresets.end()) {
                        auto* gameInstance     = *(TimeOfDayLighting**)(lightingDefinitionAddresses[lightingIndex] + 0x18);
                        auto* settingsInstance = &iter->second;

                        memcpy_s(gameInstance->SpecularColour, sizeof(float) * 4, settingsInstance->SpecularColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->DiffuseColour, sizeof(float) * 4, settingsInstance->DiffuseColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->AmbientColour, sizeof(float) * 4, settingsInstance->AmbientColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->FogSkyColour, sizeof(float) * 4, settingsInstance->FogSkyColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->FogHazeColour, sizeof(float) * 4, settingsInstance->FogHazeColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->FixedFunctionSkyColour, sizeof(float) * 4, settingsInstance->FixedFunctionSkyColour, sizeof(float) * 4);
                        gameInstance->FogDistanceScale   = settingsInstance->FogDistanceScale;
                        gameInstance->FogSkyColourScale  = settingsInstance->FogSkyColourScale;
                        gameInstance->FogHazeColourScale = settingsInstance->FogHazeColourScale;
                        gameInstance->EnvSkyBrightness   = settingsInstance->EnvSkyBrightness;
                        gameInstance->CarSpecScale       = settingsInstance->CarSpecScale;

                        *(float*)(lightingDefinitionAddresses[lightingIndex] + 0x40) = settingsInstance->FogInLightScatter;
                        *(float*)(lightingDefinitionAddresses[lightingIndex] + 0x4C) = settingsInstance->FogSunFalloff;
                     }
                  }
                  lightingIndex++;
               }

               step += 0xC;
               Sleep(1);
            }

            /*for (auto const& lightingDefinition : lightingDefinitionAddresses) {
               if (*curLighting_pt1 == lightingDefinition.second) {
                  currentLightingIndex = lightingDefinition.first;

                  if (Settings::settingsType.todPresets.size() != 0) {
                     std::string strHash = lightingHashes[currentLightingIndex];
                     auto iter = Settings::settingsType.todPresets.find(strHash);
                     if (iter != Settings::settingsType.todPresets.end()) {
                        TimeOfDayLightingPreset* todSettings = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];
                     }
                  }
                  break;
               }
            }*/

            todInstance = *(TimeOfDay**)(0x5B392C + 0x400000);
            hasLoadedData = true;
         }

         const virtual void onFrame() override {}

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            return ImGui::Button("Time of Day Editor", buttonSize);
         }
         const virtual bool displayMenu() override {
            curLighting_pt1 = Memory::readPointer(0x5B392C, 1, 0xC4);
            curLighting_pt2 = Memory::readPointer(0x5B392C, 1, 0xC8);
            if (curLighting_pt1 && curLighting_pt2) {
               if (!todLightingInstance)
                  todLightingInstance = *(TimeOfDayLighting**)(*curLighting_pt1 + 0x18);
               if (!todLightingSettingsInstance)
                  todLightingSettingsInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];
               if (todLightingInstance && todLightingSettingsInstance) {
                  ImGui::Text("Current lighting:"); ImGui::SameLine();
                  ImGui::PushItemWidth(-1.0f);
                  if (ImGui::Combo("##CurLighting", &currentLightingIndex, lightingHashes)) {
                     *curLighting_pt1 = lightingDefinitionAddresses[currentLightingIndex];
                     *curLighting_pt2 = *(DWORD*)(*curLighting_pt1 + 0x18);

                     todLightingInstance         = *(TimeOfDayLighting**)(*curLighting_pt1 + 0x18);
                     todLightingSettingsInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];

                     *(float*)(*curLighting_pt1 + 0x40) = todLightingSettingsInstance->FogInLightScatter;
                     *(float*)(*curLighting_pt1 + 0x4C) = todLightingSettingsInstance->FogSunFalloff;
                  }
                  ImGui::PopItemWidth();

                  if (ImGui::Button("Save preset")) {
                     todLightingSettingsInstance->FogInLightScatter = *(float*)(*curLighting_pt1 + 0x40);
                     todLightingSettingsInstance->FogSunFalloff     = *(float*)(*curLighting_pt1 + 0x4C);
                     *todLightingSettingsInstance                   = todLightingInstance;
                     Settings::saveSettings();
                  } ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                  if (ImGui::Button("Save preset for all")) {
                     // save presets
                     for (auto& preset : Settings::settingsType.todPresets) {
                        preset.second.FogInLightScatter = *(float*)(*curLighting_pt1 + 0x40);
                        preset.second.FogSunFalloff     = *(float*)(*curLighting_pt1 + 0x4C);
                        preset.second                   = todLightingInstance;
                        Settings::saveSettings();
                     }
                     // apply to all
                     for (auto& lightingDef : lightingDefinitionAddresses) {
                        DWORD lighting = lightingDef.second;

                        TimeOfDayLighting* listTodInstance = *(TimeOfDayLighting**)(lighting + 0x18);
                        *listTodInstance = todLightingInstance;

                        *(float*)(lighting + 0x40) = *(float*)(*curLighting_pt1 + 0x40);
                        *(float*)(lighting + 0x4C) = *(float*)(*curLighting_pt1 + 0x4C);
                     }
                  }
                  ImGui::Separator();
                  float lineDiff = ImGui::CalcTextSize("FixedFunctionSkyColour:").x + ImGui::GetStyle().WindowPadding.x;
                  ImGui::PushItemWidth(lineDiff);
                  ImGui::Text("SpecularColour"); ImGui::SameLine(lineDiff); ImGui::ColorEdit4("##SpecularColour", todLightingInstance->SpecularColour);
                  ImGui::Text("DiffuseColour"); ImGui::SameLine(lineDiff); ImGui::ColorEdit4("##DiffuseColour", todLightingInstance->DiffuseColour);
                  ImGui::Text("AmbientColour"); ImGui::SameLine(lineDiff); ImGui::ColorEdit4("##AmbientColour", todLightingInstance->AmbientColour);
                  ImGui::Text("FogHazeColour"); ImGui::SameLine(lineDiff); ImGui::ColorEdit4("##FogHazeColour", todLightingInstance->FogHazeColour);
                  ImGui::Text("FixedFunctionSkyColour"); ImGui::SameLine(lineDiff); ImGui::ColorEdit4("##FixedFunctionSkyColour", todLightingInstance->FixedFunctionSkyColour);
                  ImGui::Text("FogDistanceScale"); ImGui::SameLine(lineDiff); ImGui::DragFloat("##FogDistanceScale", &todLightingInstance->FogDistanceScale, 10.0f, -100.0f, 1000.0f);
                  ImGui::Text("FogHazeColourScale"); ImGui::SameLine(lineDiff); ImGui::DragFloat("##FogHazeColourScale", &todLightingInstance->FogHazeColourScale, 10.0f, 0.0f, 1000.0f);
                  ImGui::Text("FogSkyColourScale"); ImGui::SameLine(lineDiff); ImGui::DragFloat("##FogSkyColourScale", &todLightingInstance->FogSkyColourScale, 10.0f, 0.0f, 1000.0f);
                  ImGui::Text("EnvSkyBrightness"); ImGui::SameLine(lineDiff); ImGui::SliderFloat("##EnvSkyBrightness", &todLightingInstance->EnvSkyBrightness, 0.0f, 10.0f);
                  ImGui::Text("CarSpecScale"); ImGui::SameLine(lineDiff); ImGui::DragFloat("##CarSpecScale", &todLightingInstance->CarSpecScale, 0.25f, 0.0f, 100.0f);
                  ImGui::Text("FogSkyColour"); ImGui::SameLine(lineDiff); ImGui::ColorEdit4("##FogSkyColour", todLightingInstance->FogSkyColour);

                  ImGui::Text("FogInLightScatter"); ImGui::SameLine(lineDiff); ImGui::SliderFloat("##FogInLightScatter", (float*)(*curLighting_pt1 + 0x40), 0.0f, 100.0f);
                  ImGui::Text("FogSunFalloff"); ImGui::SameLine(lineDiff); ImGui::SliderFloat("##FogSunFalloff", (float*)(*curLighting_pt1 + 0x4C), 0.0f, 100.0f);
                  ImGui::PopItemWidth();
               } else {
                  ImGui::Text("There was an issue...");
               }
               ImGui::Separator();
               ImGui::Text("Time of Day");
               ImGui::SliderFloat("skybox speed multiplier", &todInstance->SkyboxSpeedMultiplier, -100.0f, 100.0f);
               ImGui::SliderInt("sun speed multiplier (w/ correlation to skybox speed)", &todInstance->SunSpeedMultiplier, -100.0f, 100.0f);
               ImGui::SliderFloat("tod value", &todInstance->ToDValue, 0.0f, 1.0f);
               ImGui::SliderAngle("sunangle", &todInstance->SunAngleInRads);
               ImGui::SliderAngle("sunpos", &todInstance->SunPositionInRads);
               return false;
            } else {
               return true;
            }
         }
      };
   }
}