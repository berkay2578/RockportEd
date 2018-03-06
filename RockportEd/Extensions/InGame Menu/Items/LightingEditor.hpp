#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::TimeOfDayLighting;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      struct LightingEditor : _BaseInGameMenuItem {
      private:
         DWORD * curLighting_pt1 = nullptr;
         DWORD* curLighting_pt2 = nullptr;
         //BYTE*  renderSun_code  = nullptr;

         std::vector<std::string> lightingHashes              = std::vector<std::string>();
         std::map<int, DWORD>     lightingDefinitionAddresses = {};
         int                      currentLightingIndex        = 0;

         TimeOfDayLighting*       todInstance         = nullptr;
         TimeOfDayLightingPreset* todSettingsInstance = nullptr;

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

            //renderSun_code = (BYTE*)Memory::makeAbsolute(0x2D0516 + 0x1);

            /*for (auto const& lightingDefinition : lightingDefinitionAddresses) {
               if (*curLighting_pt1 == lightingDefinition.second) {
                  currentLightingIndex = lightingDefinition.first;

                  if (Settings::settingsType.todPresets.size() != 0) {
                     std::string strHash = lightingHashes[currentLightingIndex];
                     auto iter = Settings::settingsType.todPresets.find(strHash);
                     if (iter != Settings::settingsType.todPresets.end()) {
                        TimeOfDayLightingPreset* todSettings = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];
                        //*renderSun_code = todSettings->RenderSun ? 0x85 : 0x84;
                     }
                  }
                  break;
               }
            }*/
            hasLoadedData = true;
         }
         const virtual void displayMenu() override {
            ImGui::Checkbox("Lighting Editor", &isEnabled);
            if (isEnabled) {
               curLighting_pt1 = Memory::readPointer(0x5B392C, 1, 0xC4);
               curLighting_pt2 = Memory::readPointer(0x5B392C, 1, 0xC8);
               if (curLighting_pt1 && curLighting_pt2) {
                  if (ImGui::Begin("Time of Day Lighting Editor", &isEnabled)) {
                     if (!todInstance)
                        todInstance = *(TimeOfDayLighting**)(*curLighting_pt1 + 0x18);
                     if (!todSettingsInstance)
                        todSettingsInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];

                     ImGui::Text("Current lighting:"); ImGui::SameLine(); ImGui::PushItemWidth(-1.0f);
                     if (ImGui::Combo("##CurLighting", &currentLightingIndex, lightingHashes)) {
                        *curLighting_pt1 = lightingDefinitionAddresses[currentLightingIndex];
                        *curLighting_pt2 = *(DWORD*)(*curLighting_pt1 + 0x18);

                        todInstance         = *(TimeOfDayLighting**)(*curLighting_pt1 + 0x18);
                        todSettingsInstance = &Settings::settingsType.todPresets[lightingHashes[currentLightingIndex]];

                        //*renderSun_code                    = todSettingsInstance->RenderSun ? 0x85 : 0x84;
                        *(float*)(*curLighting_pt1 + 0x40) = todSettingsInstance->FogInLightScatter;
                        *(float*)(*curLighting_pt1 + 0x4C) = todSettingsInstance->FogSunFalloff;
                     }
                     ImGui::PopItemWidth();

                     /*if (ImGui::Checkbox("Render the sun", &todSettingsInstance->RenderSun)) {
                        *renderSun_code = todSettingsInstance->RenderSun ? 0x85 : 0x84;
                     }
                     ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();*/
                     if (ImGui::Button("Save Preset")) {
                        todSettingsInstance->FogInLightScatter = *(float*)(*curLighting_pt1 + 0x40);
                        todSettingsInstance->FogSunFalloff     = *(float*)(*curLighting_pt1 + 0x4C);
                        *todSettingsInstance                   = todInstance;
                        Settings::saveSettings();
                     }
                     ImGui::Separator();

                     ImGui::ColorEdit4("SpecularColour", todInstance->SpecularColour);
                     ImGui::ColorEdit4("DiffuseColour", todInstance->DiffuseColour);
                     ImGui::ColorEdit4("AmbientColour", todInstance->AmbientColour);
                     ImGui::ColorEdit4("FogHazeColour", todInstance->FogHazeColour);
                     ImGui::ColorEdit4("FixedFunctionSkyColour", todInstance->FixedFunctionSkyColour);
                     ImGui::DragFloat("FogDistanceScale", &todInstance->FogDistanceScale, 10.0f, -100.0f, 1000.0f);
                     ImGui::DragFloat("FogHazeColourScale", &todInstance->FogHazeColourScale, 10.0f, 0.0f, 1000.0f);
                     ImGui::DragFloat("FogSkyColourScale", &todInstance->FogSkyColourScale, 10.0f, 0.0f, 1000.0f);
                     ImGui::SliderFloat("EnvSkyBrightness", &todInstance->EnvSkyBrightness, 0.0f, 10.0f);
                     ImGui::DragFloat("CarSpecScale", &todInstance->CarSpecScale, 0.25f, 0.0f, 100.0f);
                     ImGui::ColorEdit4("FogSkyColour", todInstance->FogSkyColour);

                     ImGui::SliderFloat("FogInLightScatter", (float*)(*curLighting_pt1 + 0x40), 0.0f, 100.0f);
                     ImGui::SliderFloat("FogSunFalloff", (float*)(*curLighting_pt1 + 0x4C), 0.0f, 100.0f);
                  }
                  ImGui::End();
               }
            }
         }
      };
   }
}