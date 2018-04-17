#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::CarPhysicsTuning;
using GameInternals::ObjectData;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      class CarConfigurator : public _BaseInGameMenuItem {
         CarPhysicsTuning carPhysicsTuning = { 0 };
         ObjectData*      carObjectData    = nullptr;
         bool             autoUpdate       = false;

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }

         const virtual void onFrame() override {}

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            return ImGui::Button("Car Configurator", buttonSize);
         }
         const virtual bool displayMenu() override {
            if (GameInternals::Gameplay::Object::getObjectData(carObjectData)) {
               ImGui::TextWrapped("Presets");
               ImGui::Indent(5.0f);
               {
                  static bool showPresetPopup = false;
                  showPresetPopup |= ImGui::Button("Save preset");
                  if (showPresetPopup) {
                     ImGui::OpenPopup("##SavePresetPopup_CarConfigurator");
                     static Settings::CarConfigurationPreset carConfigPreset = { 0 };
                     if (ImGui::BeginPopup("##SavePresetPopup_CarConfigurator", ImGuiWindowFlags_AlwaysAutoResize)) {
                        static char presetName[64] = { 0 };
                        ImGui::Text("Preset name: "); ImGui::SameLine();
                        ImGui::InputText("##PresetName", presetName, sizeof(presetName), ImGuiInputTextFlags_CharsNoBlank);
                        ImGui::SameLine();
                        if (ImGui::Button("Save")) {
                           carConfigPreset.Gravity       = carObjectData->Gravity;
                           carConfigPreset.Grip          = carObjectData->Grip;
                           carConfigPreset.PhysicsTuning = &carPhysicsTuning;

                           Settings::settingsType.carConfigPresets[std::string(presetName)] = carConfigPreset;
                           Settings::saveSettings();

                           ZeroMemory(&carConfigPreset, sizeof(Settings::CarConfigurationPreset));
                           ZeroMemory(presetName, sizeof(presetName));

                           showPresetPopup = false;
                        }

                        ImGui::EndPopup();
                     }
                  }
                  if (!Settings::settingsType.carConfigPresets.empty()) {
                     static Settings::CarConfigurationPreset* pActivePreset = nullptr;
                     static const char* activePreset_NamePreview            = nullptr;
                     if (!pActivePreset || !activePreset_NamePreview) {
                        pActivePreset            = &Settings::settingsType.carConfigPresets.begin()->second;
                        activePreset_NamePreview = Settings::settingsType.carConfigPresets.begin()->first.c_str();
                     }

                     if (ImGui::BeginCombo("##Presets", activePreset_NamePreview, ImGuiComboFlags_HeightSmall)) {
                        for (auto& preset : Settings::settingsType.carConfigPresets) {
                           bool isItemSelected = (preset.first == activePreset_NamePreview);
                           if (ImGui::Selectable(preset.first.c_str(), isItemSelected)) {
                              pActivePreset            = &preset.second;
                              activePreset_NamePreview = preset.first.c_str();
                           }
                           if (isItemSelected)
                              ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                     } ImGui::SameLine();
                     if (ImGui::Button("Load")) {
                        carObjectData->Gravity = pActivePreset->Gravity;
                        carObjectData->Grip    = pActivePreset->Grip;
                        memcpy_s(&carPhysicsTuning, sizeof(GameInternals::CarPhysicsTuning),
                           &pActivePreset->PhysicsTuning, sizeof(Settings::CarPhysicsTuningPreset));

                        GameInternals::Gameplay::Player::Car::setCarPhysicsTuning(&carPhysicsTuning);
                        carObjectData->z_Velocity += 1.5f;
                     } ImGui::SameLine();
                     if (ImGui::Button("Delete")) {
                        Settings::settingsType.carConfigPresets.erase(std::string(activePreset_NamePreview));
                        Settings::saveSettings();

                        if (!Settings::settingsType.carConfigPresets.empty()) {
                           pActivePreset            = &Settings::settingsType.carConfigPresets.begin()->second;
                           activePreset_NamePreview = Settings::settingsType.carConfigPresets.begin()->first.c_str();
                        } else {
                           pActivePreset            = nullptr;
                           activePreset_NamePreview = nullptr;
                        }
                     }
                  }
               }
               ImGui::Unindent(5.0f);
               ImGui::Separator();

               ImGui::TextWrapped("Object Data");
               ImGui::Indent(5.0f);
               {
                  ImGui::SliderFloat("Gravity", &carObjectData->Gravity, 0.1f, 3500.0f);
                  ImGui::SliderFloat("Grip", &carObjectData->Grip, 0.0001f, 0.003f, "%.6f");
               }
               ImGui::Unindent(5.0f);

               ImGui::TextWrapped("Physics Tuning");
               ImGui::Indent(5.0f);
               {
                  ImGui::SliderFloat("Steering", &carPhysicsTuning.Steering, -10.0f, 10.0f);
                  ImGui::SliderFloat("Handling", &carPhysicsTuning.Handling, -10.0f, 10.0f);
                  ImGui::SliderFloat("Brakes", &carPhysicsTuning.Brakes, -10.0f, 10.0f);
                  ImGui::SliderFloat("Ride Height", &carPhysicsTuning.RideHeight, -10.0f, 10.0f);
                  ImGui::SliderFloat("Aerodynamics", &carPhysicsTuning.Aerodynamics, -10.0f, 10.0f);
                  ImGui::SliderFloat("NOS", &carPhysicsTuning.NOS, -10.0f, 10.0f);
                  ImGui::SliderFloat("Turbo", &carPhysicsTuning.Turbo, -10.0f, 10.0f);
                  if (ImGui::Button("Apply") || autoUpdate) {
                     GameInternals::Gameplay::Player::Car::setCarPhysicsTuning(&carPhysicsTuning);
                     if (!autoUpdate)
                        carObjectData->z_Velocity += 1.5f;
                  } ImGui::SameLine();
                  ImGui::Checkbox("Auto Update", &autoUpdate);
               }
               ImGui::Unindent(5.0f);
               return true;
            } else {
               return false;
            }
         }
      };
   }
}