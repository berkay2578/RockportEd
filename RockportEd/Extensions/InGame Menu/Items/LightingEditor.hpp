#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::TimeOfDay;
using GameInternals::TimeOfDayLighting_Memory;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

/*
HUGE BUG
BECAUSE I FORCE SUNFALLOFF AND THE OTHER VALUE MANUALLY, TODSETTINGSINSTANCE IS CREATED WITH 0 AND IS FORCED TO THEM
I HAVE TO CHANGE THE STRUCT OF SETTINGS AND TIMEOFDAYLIGHTING TO HAVE THEM DONE AUTOMATICALLY
SO THAT ROCKPORTED IS SELF-AWARE AND DOESNT FORCE 0 TO THEM
*/

namespace Extensions {
   namespace InGameMenu {
      class TimeOfDayEditor : public _BaseInGameMenuItem {
         DWORD* curLighting_pt1 = nullptr;
         DWORD* curLighting_pt2 = nullptr;

         std::vector<std::string> lightingHashes              = std::vector<std::string>();
         std::map<int, DWORD>     lightingDefinitionAddresses = {};
         int                      currentLightingIndex        = 7;

         TimeOfDay*                todInstance                 = nullptr;
         TimeOfDayLighting_Memory* todLightingMemoryInstance   = nullptr;
         TimeOfDayLightingPreset*  todLightingSettingsInstance = nullptr;

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
                        auto* gameInstance     = *reinterpret_cast<TimeOfDayLighting_Memory**>(lightingDefinitionAddresses[lightingIndex]);
                        auto* settingsInstance = &iter->second;

                        memcpy_s(gameInstance->TimeOfDayLightingInstance->SpecularColour, sizeof(float) * 4, settingsInstance->SpecularColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->TimeOfDayLightingInstance->DiffuseColour, sizeof(float) * 4, settingsInstance->DiffuseColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->TimeOfDayLightingInstance->AmbientColour, sizeof(float) * 4, settingsInstance->AmbientColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->TimeOfDayLightingInstance->FogSkyColour, sizeof(float) * 4, settingsInstance->FogSkyColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->TimeOfDayLightingInstance->FogHazeColour, sizeof(float) * 4, settingsInstance->FogHazeColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->TimeOfDayLightingInstance->FixedFunctionSkyColour, sizeof(float) * 4, settingsInstance->FixedFunctionSkyColour, sizeof(float) * 4);
                        gameInstance->TimeOfDayLightingInstance->FogDistanceScale   = settingsInstance->FogDistanceScale;
                        gameInstance->TimeOfDayLightingInstance->FogSkyColourScale  = settingsInstance->FogSkyColourScale;
                        gameInstance->TimeOfDayLightingInstance->FogHazeColourScale = settingsInstance->FogHazeColourScale;
                        gameInstance->TimeOfDayLightingInstance->EnvSkyBrightness   = settingsInstance->EnvSkyBrightness;
                        gameInstance->TimeOfDayLightingInstance->CarSpecScale       = settingsInstance->CarSpecScale;
                        gameInstance->FogInLightScatter                             = settingsInstance->FogInLightScatter;
                        gameInstance->FogSunFalloff                                 = settingsInstance->FogSunFalloff;
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

            bool* TimeOfDaySwapEnable = (bool*)(0x4F86E8 + 0x400000);
            Memory::writeRaw((DWORD)TimeOfDaySwapEnable, true, 1, 0x0);
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
               if (!todLightingMemoryInstance)
                  todLightingMemoryInstance = *reinterpret_cast<TimeOfDayLighting_Memory**>(curLighting_pt1);
               if (!todLightingSettingsInstance)
                  todLightingSettingsInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];
               if (todLightingMemoryInstance && todLightingSettingsInstance) {
                  ImGui::Text("Current lighting:"); ImGui::SameLine();
                  ImGui::PushItemWidth(-1.0f);
                  if (ImGui::Combo("##CurLighting", &currentLightingIndex, lightingHashes)) {
                     *curLighting_pt1 = lightingDefinitionAddresses[currentLightingIndex];
                     *curLighting_pt2 = *(DWORD*)(*curLighting_pt1 + 0x18);

                     todLightingMemoryInstance   = *reinterpret_cast<TimeOfDayLighting_Memory**>(curLighting_pt1);
                     todLightingSettingsInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];
                  }
                  ImGui::PopItemWidth();

                  if (ImGui::Button("Save preset")) {
                     todLightingSettingsInstance->FogInLightScatter = todLightingMemoryInstance->FogInLightScatter;
                     todLightingSettingsInstance->FogSunFalloff     = todLightingMemoryInstance->FogSunFalloff;
                     *todLightingSettingsInstance                   = todLightingMemoryInstance->TimeOfDayLightingInstance;
                     Settings::saveSettings();
                  } ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
                  if (ImGui::Button("Save preset for all")) {
                     // save presets
                     for (auto& preset : Settings::settingsType.todPresets) {
                        preset.second.FogInLightScatter = todLightingMemoryInstance->FogInLightScatter;
                        preset.second.FogSunFalloff     = todLightingMemoryInstance->FogSunFalloff;
                        preset.second                   = todLightingMemoryInstance->TimeOfDayLightingInstance;
                        Settings::saveSettings();
                     }
                     // apply to all
                     for (auto& lightingDef : lightingDefinitionAddresses) {
                        DWORD lighting = lightingDef.second;

                        TimeOfDayLighting_Memory* listTodInstance   = *reinterpret_cast<TimeOfDayLighting_Memory**>(lighting);
                        *listTodInstance->TimeOfDayLightingInstance = todLightingMemoryInstance->TimeOfDayLightingInstance;
                        listTodInstance->FogInLightScatter          = todLightingMemoryInstance->FogInLightScatter;
                        listTodInstance->FogSunFalloff              = todLightingMemoryInstance->FogSunFalloff;
                     }
                  }
                  ImGui::Separator();
                  float lineDiff = ImGui::CalcTextSize("FixedFunctionSkyColour:").x + ImGui::GetStyle().WindowPadding.x;
                  ImGui::PushItemWidth(lineDiff);

                  ImGui::Text("SpecularColour"); ImGui::SameLine(lineDiff);
                  ImGui::ColorEdit4("##SpecularColour", todLightingMemoryInstance->TimeOfDayLightingInstance->SpecularColour);

                  ImGui::Text("DiffuseColour"); ImGui::SameLine(lineDiff);
                  ImGui::ColorEdit4("##DiffuseColour", todLightingMemoryInstance->TimeOfDayLightingInstance->DiffuseColour);

                  ImGui::Text("AmbientColour"); ImGui::SameLine(lineDiff);
                  ImGui::ColorEdit4("##AmbientColour", todLightingMemoryInstance->TimeOfDayLightingInstance->AmbientColour);

                  ImGui::Text("FogHazeColour"); ImGui::SameLine(lineDiff);
                  ImGui::ColorEdit4("##FogHazeColour", todLightingMemoryInstance->TimeOfDayLightingInstance->FogHazeColour);

                  ImGui::Text("FixedFunctionSkyColour"); ImGui::SameLine(lineDiff);
                  ImGui::ColorEdit4("##FixedFunctionSkyColour", todLightingMemoryInstance->TimeOfDayLightingInstance->FixedFunctionSkyColour);

                  ImGui::Text("FogDistanceScale"); ImGui::SameLine(lineDiff);
                  ImGui::DragFloat("##FogDistanceScale", &todLightingMemoryInstance->TimeOfDayLightingInstance->FogDistanceScale, 10.0f, -100.0f, 1000.0f);

                  ImGui::Text("FogHazeColourScale"); ImGui::SameLine(lineDiff);
                  ImGui::DragFloat("##FogHazeColourScale", &todLightingMemoryInstance->TimeOfDayLightingInstance->FogHazeColourScale, 10.0f, 0.0f, 1000.0f);

                  ImGui::Text("FogSkyColourScale"); ImGui::SameLine(lineDiff);
                  ImGui::DragFloat("##FogSkyColourScale", &todLightingMemoryInstance->TimeOfDayLightingInstance->FogSkyColourScale, 10.0f, 0.0f, 1000.0f);

                  ImGui::Text("EnvSkyBrightness"); ImGui::SameLine(lineDiff);
                  ImGui::SliderFloat("##EnvSkyBrightness", &todLightingMemoryInstance->TimeOfDayLightingInstance->EnvSkyBrightness, 0.0f, 10.0f);

                  ImGui::Text("CarSpecScale"); ImGui::SameLine(lineDiff);
                  ImGui::DragFloat("##CarSpecScale", &todLightingMemoryInstance->TimeOfDayLightingInstance->CarSpecScale, 0.25f, 0.0f, 100.0f);

                  ImGui::Text("FogSkyColour"); ImGui::SameLine(lineDiff);
                  ImGui::ColorEdit4("##FogSkyColour", todLightingMemoryInstance->TimeOfDayLightingInstance->FogSkyColour);

                  ImGui::Text("FogInLightScatter"); ImGui::SameLine(lineDiff);
                  ImGui::SliderFloat("##FogInLightScatter", &todLightingMemoryInstance->FogInLightScatter, 0.0f, 100.0f);

                  ImGui::Text("FogSunFalloff"); ImGui::SameLine(lineDiff);
                  ImGui::SliderFloat("##FogSunFalloff", &todLightingMemoryInstance->FogSunFalloff, 0.0f, 100.0f);

                  ImGui::PopItemWidth();
               } else {
                  ImGui::Text("There was an issue...");
               }
               ImGui::Separator();
               ImGui::Text("Time of Day");
               ImGui::SliderFloat("sky speed-mult", &todInstance->SkyboxSpeedMultiplier, -100.0f, 100.0f);
               ImGui::SliderInt("tod speed-mult", &todInstance->TimeOfDaySpeedMultiplier, -100, 100);
               ImGui::SliderFloat("tod value", &todInstance->TimeOfDayValue, 0.0f, 1.0f);
               ImGui::SliderAngle("sun-orbitangle", &todInstance->SunOrbitAngleInRads);
               ImGui::SliderAngle("sun-orbitpos", &todInstance->SunOrbitPositionInRads);
               return false;
            } else {
               return true;
            }
         }
      };
   }
}