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
               return false;
            } else {
               return true;
            }
         }
      };
   }
}