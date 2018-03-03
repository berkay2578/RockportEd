#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::CarPhysicsTuning;
using GameInternals::ObjectData;
// ImGui::VerticalSeparator
#include "Helpers\imgui\imgui_internal.h"

namespace Extensions {
   namespace InGameMenu {
      struct CarConfigurator : _BaseInGameMenuItem {
      private:
         CarPhysicsTuning carPhysicsTuning = { 0 };
         ObjectData*      carObjectData    = nullptr;
         bool             autoUpdate       = false;

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }
         const virtual void displayMenu() override {
            ImGui::Checkbox("Car Configurator", &isEnabled);
            if (isEnabled) {
               if (GameInternals::Gameplay::Object::getObjectData(carObjectData)) {
                  if (ImGui::Begin("Car Configurator", &isEnabled)) {
                     ImGui::TextWrapped("Object Data");
                     ImGui::Indent(5.0f);
                     {
                        ImGui::SliderFloat("Gravity", &carObjectData->gravity, 0.1f, 3500.0f);
                        ImGui::SliderFloat("Grip", &carObjectData->grip, 0.0001f, 0.003f, "%.6f");
                     }
                     ImGui::Unindent(5.0f);

                     ImGui::TextWrapped("Physics Tuning");
                     ImGui::Indent(5.0f);
                     {
                        ImGui::SliderFloat("Steering", &carPhysicsTuning.steering, -10.0f, 10.0f);
                        ImGui::SliderFloat("Handling", &carPhysicsTuning.handling, -10.0f, 10.0f);
                        ImGui::SliderFloat("Brakes", &carPhysicsTuning.brakes, -10.0f, 10.0f);
                        ImGui::SliderFloat("Ride Height", &carPhysicsTuning.rideHeight, -10.0f, 10.0f);
                        ImGui::SliderFloat("Aerodynamics", &carPhysicsTuning.aerodynamics, -10.0f, 10.0f);
                        ImGui::SliderFloat("NOS", &carPhysicsTuning.nos, -10.0f, 10.0f);
                        ImGui::SliderFloat("Turbo", &carPhysicsTuning.turbo, -10.0f, 10.0f);
                        if (ImGui::Button("Apply") || autoUpdate) {
                           GameInternals::Gameplay::Player::Car::setCarPhysicsTuning(&carPhysicsTuning);
                           if (!autoUpdate)
                              carObjectData->z_Velocity += 1.5f;
                        } ImGui::SameLine();
                        ImGui::Checkbox("Auto Update", &autoUpdate);
                     }
                     ImGui::Unindent(5.0f);
                  }
                  ImGui::End();
               }
            }
         }
      };
   }
}