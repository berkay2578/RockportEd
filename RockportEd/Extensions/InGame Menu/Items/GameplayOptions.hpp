#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"

namespace Extensions {
   namespace InGameMenu {
      class GameplayOptions : public _BaseInGameMenuItem {
         float* trafficRefreshInterval = nullptr;

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }

         const virtual void onFrame() override {}

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            return ImGui::Button("Gameplay Options", buttonSize);
         }
         const virtual bool displayMenu() override {
            ImGui::PushItemWidth(-1.0f);

            // Traffic refresh interval
            {
               ImGui::TextWrapped("Traffic refresh interval (in seconds)");
               trafficRefreshInterval = GameInternals::Gameplay::Variables::getTrafficRefreshIntervalAsPointer();
               if (trafficRefreshInterval) {
                  ImGui::SliderFloat("##TrafficRefreshInterval", trafficRefreshInterval, 1.0f, 640.0f);
               } else {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  ImGui::TextWrapped("Traffic AI not yet initialized by the game.");
                  ImGui::PopStyleColor();
               }
            }

            ImGui::PopItemWidth();
            return false;
         }
      };
   }
}