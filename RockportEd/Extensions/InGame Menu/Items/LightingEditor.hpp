#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::TimeOfDay;
using GameInternals::TimeOfDayLighting_Memory;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      class TimeOfDayEditor : public _BaseInGameMenuItem {
         std::vector<std::string>                 lightingHashes       = std::vector<std::string>();
         std::map<int, TimeOfDayLighting_Memory*> lightingDefinitions  = {};
         int                                      currentLightingIndex = 7;

         TimeOfDay*                todInstance                 = nullptr;
         TimeOfDayLighting_Memory* todLightingEditInstance     = nullptr;

      public:
         const virtual void loadData() override {
            const DWORD* todLightingsArray = nullptr;
            while (!todLightingsArray) {
               todLightingsArray = Memory::readPointer(0x5B392C, 3, 0xEC, 0x14, 0x1C);
               Sleep(10);
            }
            todInstance = *(TimeOfDay**)(0x5B392C + 0x400000);

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

                  lightingDefinitions[lightingIndex] = *reinterpret_cast<TimeOfDayLighting_Memory**>(*todLightingsArray + step + 0x4);
                  if (Settings::settingsType.todPresets.size() != 0) {
                     auto iter = Settings::settingsType.todPresets.find(strHash_proper);
                     if (iter != Settings::settingsType.todPresets.end()) {
                        auto* gameInstance     = lightingDefinitions[lightingIndex];
                        auto* settingsInstance = &iter->second;

                        memcpy_s(gameInstance->LightingData->SpecularColour, sizeof(float) * 4, settingsInstance->SpecularColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->LightingData->DiffuseColour, sizeof(float) * 4, settingsInstance->DiffuseColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->LightingData->AmbientColour, sizeof(float) * 4, settingsInstance->AmbientColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->LightingData->FogSkyColour, sizeof(float) * 4, settingsInstance->FogSkyColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->LightingData->FogHazeColour, sizeof(float) * 4, settingsInstance->FogHazeColour, sizeof(float) * 4);
                        memcpy_s(gameInstance->LightingData->FixedFunctionSkyColour, sizeof(float) * 4, settingsInstance->FixedFunctionSkyColour, sizeof(float) * 4);
                        gameInstance->LightingData->FogDistanceScale   = settingsInstance->FogDistanceScale;
                        gameInstance->LightingData->FogSkyColourScale  = settingsInstance->FogSkyColourScale;
                        gameInstance->LightingData->FogHazeColourScale = settingsInstance->FogHazeColourScale;
                        gameInstance->LightingData->EnvSkyBrightness   = settingsInstance->EnvSkyBrightness;
                        gameInstance->LightingData->CarSpecScale       = settingsInstance->CarSpecScale;
                        gameInstance->FogInLightScatter                = settingsInstance->FogInLightScatter;
                        gameInstance->FogSunFalloff                    = settingsInstance->FogSunFalloff;
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

            bool* TimeOfDaySwapEnable = (bool*)(0x4F86E8 + 0x400000);
            Memory::writeRaw((DWORD)TimeOfDaySwapEnable, true, 1, 0x0);
            hasLoadedData = true;
         }

         const virtual void onFrame() override {}

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            return ImGui::Button("Time of Day Editor", buttonSize);
         }
         const virtual bool displayMenu() override {
            if (!todLightingEditInstance)
               todLightingEditInstance = lightingDefinitions[currentLightingIndex];
            if (todLightingEditInstance) {
               ImGui::Text("Current lighting data:"); ImGui::SameLine();
               if (ImGui::Combo("##CurLighting", &currentLightingIndex, lightingHashes)) {
                  todLightingEditInstance = lightingDefinitions[currentLightingIndex];
               }
               ImGui::SameLine();
               if (ImGui::Button("Set active")) {
                  todInstance->TimeOfDayLightingMemoryInstance = lightingDefinitions[currentLightingIndex];
                  todInstance->TimeOfDayLightingInstance       = todInstance->TimeOfDayLightingMemoryInstance->LightingData;
                  todInstance->TimeOfDayValue                  = 0.5f;
               }

               if (ImGui::Button("Save preset")) {
                  TimeOfDayLightingPreset* presetInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];
                  presetInstance->FogInLightScatter = todLightingEditInstance->FogInLightScatter;
                  presetInstance->FogSunFalloff     = todLightingEditInstance->FogSunFalloff;
                  *presetInstance                   = todLightingEditInstance->LightingData;
                  Settings::saveSettings();
               }
               ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
               if (ImGui::Button("Save preset for all")) {
                  for (auto& lightingDef : lightingDefinitions) {
                     // save
                     TimeOfDayLightingPreset* presetInstance = &Settings::settingsType.todPresets[lightingHashes[lightingDef.first]];
                     presetInstance->FogInLightScatter = todLightingEditInstance->FogInLightScatter;
                     presetInstance->FogSunFalloff     = todLightingEditInstance->FogSunFalloff;
                     *presetInstance                   = todLightingEditInstance->LightingData;

                     // apply
                     auto* listTimeOfDayMemoryInstance = lightingDef.second;
                     listTimeOfDayMemoryInstance->FogInLightScatter = todLightingEditInstance->FogInLightScatter;
                     listTimeOfDayMemoryInstance->FogSunFalloff     = todLightingEditInstance->FogSunFalloff;
                     *listTimeOfDayMemoryInstance->LightingData     = todLightingEditInstance->LightingData;
                  }
                  Settings::saveSettings();
               }
               ImGui::Separator();
               float lineDiff = ImGui::CalcTextSize("FixedFunctionSkyColour:").x + ImGui::GetStyle().WindowPadding.x;
               ImGui::PushItemWidth(lineDiff);

               ImGui::Text("SpecularColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##SpecularColour", todLightingEditInstance->LightingData->SpecularColour);

               ImGui::Text("DiffuseColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##DiffuseColour", todLightingEditInstance->LightingData->DiffuseColour);

               ImGui::Text("AmbientColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##AmbientColour", todLightingEditInstance->LightingData->AmbientColour);

               ImGui::Text("FogHazeColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##FogHazeColour", todLightingEditInstance->LightingData->FogHazeColour);

               ImGui::Text("FixedFunctionSkyColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##FixedFunctionSkyColour", todLightingEditInstance->LightingData->FixedFunctionSkyColour);

               ImGui::Text("FogDistanceScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##FogDistanceScale", &todLightingEditInstance->LightingData->FogDistanceScale, 10.0f, -100.0f, 1000.0f);

               ImGui::Text("FogHazeColourScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##FogHazeColourScale", &todLightingEditInstance->LightingData->FogHazeColourScale, 10.0f, 0.0f, 1000.0f);

               ImGui::Text("FogSkyColourScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##FogSkyColourScale", &todLightingEditInstance->LightingData->FogSkyColourScale, 10.0f, 0.0f, 1000.0f);

               ImGui::Text("EnvSkyBrightness"); ImGui::SameLine(lineDiff);
               ImGui::SliderFloat("##EnvSkyBrightness", &todLightingEditInstance->LightingData->EnvSkyBrightness, 0.0f, 10.0f);

               ImGui::Text("CarSpecScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##CarSpecScale", &todLightingEditInstance->LightingData->CarSpecScale, 0.25f, 0.0f, 100.0f);

               ImGui::Text("FogSkyColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##FogSkyColour", todLightingEditInstance->LightingData->FogSkyColour);

               ImGui::Text("FogInLightScatter"); ImGui::SameLine(lineDiff);
               ImGui::SliderFloat("##FogInLightScatter", &todLightingEditInstance->FogInLightScatter, -100.0f, 100.0f);

               ImGui::Text("FogSunFalloff"); ImGui::SameLine(lineDiff);
               ImGui::SliderFloat("##FogSunFalloff", &todLightingEditInstance->FogSunFalloff, -3.0f, 3.0f);

               ImGui::PopItemWidth();
            } else {
               ImGui::Text("There was an issue...");
            }
            ImGui::Separator();
            ImGui::Text("Time of Day");
            ImGui::SliderFloat("sky speed-mult", &todInstance->SkyboxSpeedMultiplier, -100.0f, 100.0f);
            ImGui::SliderInt("tod speed-mult", &todInstance->TimeOfDaySpeedMultiplier, -100, 100);
            ImGui::SliderFloat("tod value", &todInstance->TimeOfDayValue, 0.05f, 0.95f);
            ImGui::SliderAngle("sun-orbitangle", &todInstance->SunOrbitAngleInRads);
            ImGui::SliderAngle("sun-orbitpos", &todInstance->SunOrbitPositionInRads);
            return false;
         }

      };
   }
}