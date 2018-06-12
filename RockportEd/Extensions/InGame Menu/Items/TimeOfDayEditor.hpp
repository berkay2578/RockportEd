#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::TimeOfDay;
using GameInternals::TimeOfDayLighting;
using GameInternals::TimeOfDayLightingData;

namespace Extensions {
   namespace InGameMenu {
      class TimeOfDayEditor : public _BaseInGameMenuItem {
         int32_t                  curLightingIndex       = 0;
         TimeOfDayLighting*       pEditTimeOfDayLighting = nullptr;
         std::vector<std::string> vStrLightingHashes;

         TimeOfDay* pTimeOfDay = nullptr;
      public:
         const virtual void loadData() override {
            while (!pTimeOfDay) {
               pTimeOfDay = TimeOfDayInternals::getTimeOfDay();
               Sleep(100);
            }
            while (!TimeOfDayInternals::loadLightingDefinitions()) {
               Sleep(100);
            }

            int count = TimeOfDayInternals::lightingDefinitions.size();
            for (int lightingIndex = 0; lightingIndex < count; lightingIndex++)
            {
               TimeOfDayInternals::TimeOfDayLightingDefinition* pDefinition = TimeOfDayInternals::lightingDefinitions[lightingIndex];
               TimeOfDayLighting*                       pDefinitionLighting = pDefinition->pTimeOfDayLighting;

               std::string lightingHash = pDefinition->getHashAsString();
               vStrLightingHashes.push_back(lightingHash);
               if (Settings::settingsType.todPresets.size() != 0) {
                  auto iter = Settings::settingsType.todPresets.find(lightingHash);
                  if (iter != Settings::settingsType.todPresets.end()) {
                     auto* pSettingsInstance = &iter->second;

                     *pDefinitionLighting->pLightingData    = pSettingsInstance->LightingDataPreset.getGameInternalsCompliantData();
                     pDefinitionLighting->FogInLightScatter = pSettingsInstance->FogInLightScatter;
                     pDefinitionLighting->FogSunFalloff     = pSettingsInstance->FogSunFalloff;
                  }
               }

               if (pDefinitionLighting == pTimeOfDay->pTimeOfDayLighting)
                  curLightingIndex = lightingIndex;
            }
            pEditTimeOfDayLighting = TimeOfDayInternals::lightingDefinitions[curLightingIndex]->pTimeOfDayLighting;
            hasLoadedData = true;
         }

         const virtual void onFrame() override {}

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            return ImGui::Button("Time of day and lighting editor", buttonSize);
         }
         const virtual bool displayMenu() override {
            static float lineDiff = 0.0f;
            lineDiff = ImGui::CalcTextSize("Time of day progression speed multiplier...").x + ImGui::GetStyle().WindowPadding.x;
            ImGui::PushItemWidth(lineDiff * 0.625f);

            ImGui::Text("Skybox animation speed multiplier"); ImGui::SameLine(lineDiff);
            ImGui::SliderFloat("##SkySpeedMult", &pTimeOfDay->SkyboxSpeedMultiplier, -100.0f, 100.0f);

            ImGui::Text("Time of day progression speed multiplier"); ImGui::SameLine(lineDiff);
            ImGui::SliderInt("##ToDSpeedMult", &pTimeOfDay->TimeOfDaySpeedMultiplier, -100, 100);

            ImGui::Text("Time of day"); ImGui::SameLine(lineDiff);
            ImGui::SliderFloat("##ToDValue", &pTimeOfDay->TimeOfDayValue, 0.05f, 0.95f);

            ImGui::Text("Sun default orbit X-Axis (Horizontal)"); ImGui::SameLine(lineDiff);
            ImGui::SliderAngle("##SunOrbitXAxis", &pTimeOfDay->SunDefaultOrbitAxisX);

            ImGui::Text("Sun default orbit Y-Axis (Vertical)"); ImGui::SameLine(lineDiff);
            ImGui::SliderAngle("##SunOrbitYAxis", &pTimeOfDay->SunDefaultOrbitAxisY);
            ImGui::PopItemWidth();
            ImGui::Separator();

            if (pEditTimeOfDayLighting) {
               ImGui::Text("Lighting "); ImGui::SameLine();
               if (ImGui::Combo("##ActiveLighting", &curLightingIndex, vStrLightingHashes)) {
                  pEditTimeOfDayLighting = TimeOfDayInternals::lightingDefinitions[curLightingIndex]->pTimeOfDayLighting;
               } ImGui::SameLine();
               if (ImGui::Button("Set active")) {
                  pTimeOfDay->pTimeOfDayLighting     = pEditTimeOfDayLighting;
                  pTimeOfDay->pTimeOfDayLightingData = pEditTimeOfDayLighting->pLightingData;
                  pTimeOfDay->TimeOfDayValue         = 0.5f;
               }

               if (ImGui::Button("Save preset")) {
                  Settings::TimeOfDayLightingPreset* pSettingsInstance = &Settings::settingsType.todPresets[vStrLightingHashes[curLightingIndex]];
                  pSettingsInstance->setTo(pEditTimeOfDayLighting);
                  Settings::saveSettings();
               } ImGui::SameLine(); ImGui::VerticalSeparator(); ImGui::SameLine();
               if (ImGui::Button("Override all and save")) {
                  int index = 0;
                  for (const auto* def : TimeOfDayInternals::lightingDefinitions) {
                     // save
                     Settings::TimeOfDayLightingPreset* pSettingsInstance = &Settings::settingsType.todPresets[vStrLightingHashes[index]];
                     pSettingsInstance->setTo(pEditTimeOfDayLighting);

                     // apply
                     def->pTimeOfDayLighting->FogInLightScatter = pEditTimeOfDayLighting->FogInLightScatter;
                     def->pTimeOfDayLighting->FogSunFalloff     = pEditTimeOfDayLighting->FogSunFalloff;
                     def->pTimeOfDayLighting->pLightingData->setTo(pEditTimeOfDayLighting->pLightingData);

                     index++;
                  }
                  Settings::saveSettings();
               }

               if (ImGui::Button("Restore all to defaults")) {
                  TimeOfDayInternals::restoreAll();
                  Settings::settingsType.todPresets.clear();
                  Settings::saveSettings();
               }

               lineDiff = ImGui::CalcTextSize("FixedFunctionSkyColour...").x + ImGui::GetStyle().WindowPadding.x;
               ImGui::PushItemWidth(-1.0f);

               ImGui::Text("SpecularColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##SpecularColour", pEditTimeOfDayLighting->pLightingData->SpecularColour);

               ImGui::Text("DiffuseColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##DiffuseColour", pEditTimeOfDayLighting->pLightingData->DiffuseColour);

               ImGui::Text("AmbientColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##AmbientColour", pEditTimeOfDayLighting->pLightingData->AmbientColour);

               ImGui::Text("FogHazeColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##FogHazeColour", pEditTimeOfDayLighting->pLightingData->FogHazeColour);

               ImGui::Text("FixedFunctionSkyColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##FixedFunctionSkyColour", pEditTimeOfDayLighting->pLightingData->FixedFunctionSkyColour);

               ImGui::Text("FogDistanceScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##FogDistanceScale", &pEditTimeOfDayLighting->pLightingData->FogDistanceScale, 10.0f, -100.0f, 1000.0f);

               ImGui::Text("FogHazeColourScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##FogHazeColourScale", &pEditTimeOfDayLighting->pLightingData->FogHazeColourScale, 10.0f, 0.0f, 1000.0f);

               ImGui::Text("FogSkyColourScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##FogSkyColourScale", &pEditTimeOfDayLighting->pLightingData->FogSkyColourScale, 10.0f, 0.0f, 1000.0f);

               ImGui::Text("EnvSkyBrightness"); ImGui::SameLine(lineDiff);
               ImGui::SliderFloat("##EnvSkyBrightness", &pEditTimeOfDayLighting->pLightingData->EnvSkyBrightness, 0.0f, 10.0f);

               ImGui::Text("CarSpecScale"); ImGui::SameLine(lineDiff);
               ImGui::DragFloat("##CarSpecScale", &pEditTimeOfDayLighting->pLightingData->CarSpecScale, 0.25f, 0.0f, 100.0f);

               ImGui::Text("FogSkyColour"); ImGui::SameLine(lineDiff);
               ImGui::ColorEdit4("##FogSkyColour", pEditTimeOfDayLighting->pLightingData->FogSkyColour);

               ImGui::Text("FogInLightScatter"); ImGui::SameLine(lineDiff);
               ImGui::SliderFloat("##FogInLightScatter", &pEditTimeOfDayLighting->FogInLightScatter, -100.0f, 100.0f);

               ImGui::Text("FogSunFalloff"); ImGui::SameLine(lineDiff);
               ImGui::SliderFloat("##FogSunFalloff", &pEditTimeOfDayLighting->FogSunFalloff, -100.0f, 100.0f);

               ImGui::PopItemWidth();
            } else {
               ImGui::Text("There was an issue setting up the lighting info.");
               pEditTimeOfDayLighting = TimeOfDayInternals::lightingDefinitions[curLightingIndex]->pTimeOfDayLighting;
            }
            return true;
         }
      };
   }
}