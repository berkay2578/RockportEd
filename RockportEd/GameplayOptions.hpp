#pragma once
#include "stdafx.h"
#include "Extensions.h"

namespace Extensions {
   namespace InGameMenu {
      struct GameplayOptions : _BaseInGameMenuItem {
      private:
         float* trafficRefreshInterval = nullptr;

      public:
         const virtual void loadData() override {
            hasLoadedData = true;
         }
         const virtual void displayMenu() override {
            ImGui::Checkbox("Gameplay Options", &isEnabled);
            if (isEnabled) {
               ImGui::SetNextWindowPos(ImVec2(10.0f, 5.0f * 2 + 100.0f), ImGuiCond_Once);
               if (ImGui::Begin("Gameplay Options", &isEnabled)) {
                  ImGui::PushItemWidth(-1.0f);

                  // Traffic refresh interval
                  {
                     ImGui::TextWrapped("Traffic refresh interval (in seconds)");
                     trafficRefreshInterval = GameInternals::Gameplay::Variables::getTrafficRefreshIntervalAsPointer();
                     if (trafficRefreshInterval) {
                        ImGui::SliderFloat("##TrafficRefreshInterval", trafficRefreshInterval, 1.0f, 640.0f);
                     }
                     else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        ImGui::TextWrapped("Traffic AI not yet initialized by the game.");
                        ImGui::PopStyleColor();
                     }
                  }

                  ImGui::PopItemWidth();
               }
               ImGui::End();
            }
         }
      };
   }
}