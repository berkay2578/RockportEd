#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"
using GameInternals::Data::GameTypes::CarPowerData;

namespace Extensions {
   namespace InGameMenu {
      class CustomHUD : public _BaseInGameMenuItem {
         bool isEnabled        = false;

         Settings::HUDColourPreset  activeHUDColourPreset = {};

         Settings::HUDColourPreset*         pActivePreset     = nullptr;
         const char*                        pActivePresetName = nullptr;
         RockportEdControls::PresetControls presetControls;

      public:
         bool createPreset(const char* presetName) {
            auto stlString = std::string(presetName);
            Settings::settingsType.colours.hudColourPresets[stlString] = activeHUDColourPreset;
            Settings::saveSettings();

            // TODO: Improve preset name loading
            pActivePreset = &Settings::settingsType.colours.hudColourPresets[stlString];
            for (auto& preset : Settings::settingsType.colours.hudColourPresets) {
               if (&preset.second == pActivePreset)
                  pActivePresetName = preset.first.c_str();
            }
            return pActivePreset && pActivePresetName;
         }

         bool loadPreset(LPVOID* pListActivePreset, const char** /*pListActivePresetName*/) {
            activeHUDColourPreset = *reinterpret_cast<Settings::HUDColourPreset*>(*pListActivePreset);
            return true;
         }

         bool updateActivePreset() {
            Settings::settingsType.colours.hudColourPresets[std::string(pActivePresetName)] = activeHUDColourPreset;
            return true;
         }

         bool deletePreset(LPVOID* pListActivePreset, const char** pListActivePresetName) {
            Settings::settingsType.colours.hudColourPresets.erase(std::string(*pListActivePresetName));
            Settings::saveSettings();

            if (!Settings::settingsType.colours.hudColourPresets.empty()) {
               pActivePreset     = &Settings::settingsType.colours.hudColourPresets.begin()->second;
               pActivePresetName = Settings::settingsType.colours.hudColourPresets.begin()->first.c_str();
               return true;
            } else {
               pActivePreset     = nullptr;
               pActivePresetName = nullptr;
               return false;
            }
         }

         bool listPresets(LPVOID* pListActivePreset, const char** pListActivePresetName) {
            if (!Settings::settingsType.colours.hudColourPresets.empty()) {
               if (!*pListActivePreset || !*pListActivePresetName) {
                  *pListActivePreset     = &Settings::settingsType.colours.hudColourPresets.begin()->second;
                  *pListActivePresetName = Settings::settingsType.colours.hudColourPresets.begin()->first.c_str();
               }
               if (*pListActivePreset && *pListActivePresetName) {
                  if (ImGui::BeginCombo("##Presets", *pListActivePresetName, ImGuiComboFlags_HeightSmall)) {
                     for (auto& preset : Settings::settingsType.colours.hudColourPresets) {
                        const char* presetName = preset.first.c_str();
                        bool isItemSelected = (presetName == *pListActivePresetName);
                        if (ImGui::Selectable(presetName, isItemSelected)) {
                           *pListActivePreset     = &preset.second;
                           *pListActivePresetName = presetName;
                        }
                        if (isItemSelected)
                           ImGui::SetItemDefaultFocus();
                     }
                     ImGui::EndCombo();
                  }
                  return true;
               }
            }
            return false;
         }

         const virtual void loadData() override {
            presetControls.Set([this](const char* _1) { return this->createPreset(_1); },
                               [this](LPVOID* _1, const char** _2) { return this->loadPreset(_1, _2); },
                               [this]() { return this->updateActivePreset(); },
                               [this](LPVOID* _1, const char** _2) { return this->deletePreset(_1, _2); },
                               [this](LPVOID* _1, const char** _2) { return this->listPresets(_1, _2); },
                               reinterpret_cast<LPVOID*>(&pActivePreset),
                               &pActivePresetName
            );
            hasLoadedData = true;
         }

         const virtual void onFrame() override {
            if (isEnabled && !InternalVariables::getVariable(InternalVariables::fadeScreenOn)) {
               static CarPowerData* pCarPowerData = nullptr;
               if (PlayerCarInternals::getCarPowerData(pCarPowerData)) {
                  static float scaling = 1.0f;
                  static float size    = 90.0f;

                  scaling = max(0.6f, (float)clientRect.right / 1280.0f); // optimized for 720p
                  size    = scaling * 90.0f;

                  imguiIO->Fonts->Fonts[3]->Scale = scaling * 0.8f;
                  imguiIO->Fonts->Fonts[4]->Scale = scaling * 0.8f;

                  ImGui::SetNextWindowBgAlpha(0.0f);
                  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                  if (ImGui::Begin("##HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize)) {

                  #define DegreeToRadianMultiplier 0.0174532925f

                     // Drawing variables
                     ImDrawList* draw_list = ImGui::GetWindowDrawList();
                     ImVec2      cursorPos = ImGui::GetCursorScreenPos();
                     cursorPos.x += size / 5.0f; // nos padding
                     cursorPos.y += size / 5.0f; // nos padding

                     // RPM drawing
                     {
                        static const float rpmValueMaximum             = 9000.0f;
                        //static const float rpmValueRedlineOffset       = 450.0f; not certain whether to use this
                        static const float rpmBeginningAngle_FullRange = 135.0f;
                        static const float rpmFinalAngle_FullRange     = 365.0f;
                        static const float rpmAngleDiff_BeginningToFinal_FullRange = rpmFinalAngle_FullRange - rpmBeginningAngle_FullRange;

                        const float rpmValueCurrent = PlayerCarInternals::getRPMFrom(pCarPowerData);

                        const float rpmBeginningValue_Redline             = pCarPowerData->MaximumRPM;
                        const float rpmBeginningAngle_Redline             = rpmBeginningAngle_FullRange + ((rpmBeginningValue_Redline / rpmValueMaximum) * rpmAngleDiff_BeginningToFinal_FullRange);
                        const float rpmValueDiff_BeginningToFinal_Redline = rpmValueMaximum - rpmBeginningValue_Redline;
                        const float rpmAngleDiff_BeginningToFinal_Redline = rpmFinalAngle_FullRange - rpmBeginningAngle_Redline;

                        const float rpmValueCurrent_ToAngle_FullRange   =
                           rpmBeginningAngle_FullRange +                 // Set the minimum starting angle
                           ((rpmValueCurrent
                             / rpmValueMaximum)                           // Get the percentage of the current RPM against the maximum RPM of the HUD
                            * rpmAngleDiff_BeginningToFinal_FullRange    // Calculate the final angle of the percentage
                            );
                        const float rpmValueCurrent_ToAngle_NormalRange = min(rpmBeginningAngle_Redline, rpmValueCurrent_ToAngle_FullRange);
                        const float rpmValueCurrent_ToAngle_Redline     =
                           rpmBeginningAngle_Redline +                   // Set the minimum starting angle
                           ((max(0,                                      // Wrap in max(0,x) to ensure no negatives
                                 rpmValueCurrent - rpmBeginningValue_Redline) // Offset the current RPM with beginning value of the redline
                             / rpmValueDiff_BeginningToFinal_Redline)     // Get the percentage of the calculated RPM against the total redline RPM of the HUD
                            * rpmAngleDiff_BeginningToFinal_Redline      // Calculate the final angle of the percentage
                            );

                        const float rpmBeginningAngle_FullRange_InRads         = rpmBeginningAngle_FullRange * DegreeToRadianMultiplier;
                        const float rpmFinalAngle_FullRange_InRads             = rpmFinalAngle_FullRange * DegreeToRadianMultiplier;
                        const float rpmBeginningAngle_Redline_InRads           = rpmBeginningAngle_Redline * DegreeToRadianMultiplier;
                        const float rpmValueCurrent_ToAngle_FullRange_InRads   = rpmValueCurrent_ToAngle_FullRange * DegreeToRadianMultiplier;
                        const float rpmValueCurrent_ToAngle_NormalRange_InRads = rpmValueCurrent_ToAngle_NormalRange * DegreeToRadianMultiplier;
                        const float rpmValueCurrent_ToAngle_Redline_InRads     = rpmValueCurrent_ToAngle_Redline * DegreeToRadianMultiplier;

                        // Gauge
                        {
                           // RPM drawing (Normal Range)
                           {
                              // Background
                              {
                                 draw_list->PathArcTo(
                                    ImVec2(cursorPos.x + size, cursorPos.y + size),
                                    size,
                                    activeHUDColourPreset.ClearBackgroundColours ? rpmValueCurrent_ToAngle_NormalRange_InRads : rpmBeginningAngle_FullRange_InRads,
                                    rpmBeginningAngle_Redline_InRads,
                                    32
                                 );

                                 if (activeHUDColourPreset.DrawGradients) {
                                    ImVec2 gradient_p[2] {
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmBeginningAngle_FullRange_InRads) * size,
                                          cursorPos.y + size + sinf(rpmBeginningAngle_FullRange_InRads) * size),
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmBeginningAngle_Redline_InRads) * size,
                                          cursorPos.y + size + sinf(rpmBeginningAngle_Redline_InRads) * size)
                                    };

                                    ImU32 col[2] {
                                       activeHUDColourPreset.RPMGauge_NormalRange_Background_Colour,
                                       activeHUDColourPreset.RPMGauge_NormalRange_Background_Colour_GradientEnd
                                    };
                                    draw_list->PathStrokeGradient(gradient_p, col, false, size * 0.1f);
                                 } else {
                                    draw_list->PathStroke(activeHUDColourPreset.RPMGauge_NormalRange_Background_Colour, false, size * 0.1f);
                                 }
                              }

                              // Foreground
                              {
                                 draw_list->PathArcTo(
                                    ImVec2(cursorPos.x + size, cursorPos.y + size),
                                    size,
                                    rpmBeginningAngle_FullRange_InRads,
                                    rpmValueCurrent_ToAngle_NormalRange_InRads,
                                    32
                                 );

                                 if (activeHUDColourPreset.DrawGradients) {
                                    ImVec2 gradient_p[2] {
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmBeginningAngle_FullRange_InRads) * size,
                                          cursorPos.y + size + sinf(rpmBeginningAngle_FullRange_InRads) * size),
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmValueCurrent_ToAngle_NormalRange_InRads) * size,
                                          cursorPos.y + size + sinf(rpmValueCurrent_ToAngle_NormalRange_InRads) * size)
                                    };

                                    ImU32 col[2] {
                                       activeHUDColourPreset.RPMGauge_NormalRange_Foreground_Colour,
                                       activeHUDColourPreset.RPMGauge_NormalRange_Foreground_Colour_GradientEnd
                                    };
                                    draw_list->PathStrokeGradient(gradient_p, col, false, size * 0.1f);
                                 } else {
                                    draw_list->PathStroke(activeHUDColourPreset.RPMGauge_NormalRange_Foreground_Colour, false, size * 0.1f);
                                 }
                              }
                           }
                           // RPM drawing (Redline)
                           {
                              // Background
                              {
                                 draw_list->PathArcTo(
                                    ImVec2(cursorPos.x + size, cursorPos.y + size),
                                    size,
                                    activeHUDColourPreset.ClearBackgroundColours ? rpmValueCurrent_ToAngle_Redline_InRads : rpmBeginningAngle_Redline_InRads,
                                    rpmFinalAngle_FullRange_InRads,
                                    32
                                 );

                                 if (activeHUDColourPreset.DrawGradients) {
                                    ImVec2 gradient_p[2] {
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmBeginningAngle_Redline_InRads) * size,
                                          cursorPos.y + size + sinf(rpmBeginningAngle_Redline_InRads) * size),
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmFinalAngle_FullRange_InRads) * size,
                                          cursorPos.y + size + sinf(rpmFinalAngle_FullRange_InRads) * size)
                                    };

                                    ImU32 col[2] {
                                       activeHUDColourPreset.RPMGauge_Redline_Background_Colour,
                                       activeHUDColourPreset.RPMGauge_Redline_Background_Colour_GradientEnd
                                    };
                                    draw_list->PathStrokeGradient(gradient_p, col, false, size * 0.1f);
                                 } else {
                                    draw_list->PathStroke(activeHUDColourPreset.RPMGauge_Redline_Background_Colour, false, size * 0.1f);
                                 }
                              }
                              // Foreground
                              {
                                 draw_list->PathArcTo(
                                    ImVec2(cursorPos.x + size, cursorPos.y + size),
                                    size,
                                    rpmBeginningAngle_Redline_InRads,
                                    rpmValueCurrent_ToAngle_Redline_InRads,
                                    32
                                 );

                                 if (activeHUDColourPreset.DrawGradients) {
                                    ImVec2 gradient_p[2] {
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmBeginningAngle_Redline_InRads) * size,
                                          cursorPos.y + size + sinf(rpmBeginningAngle_Redline_InRads) * size),
                                       ImVec2(
                                          cursorPos.x + size + cosf(rpmValueCurrent_ToAngle_Redline_InRads) * size,
                                          cursorPos.y + size + sinf(rpmValueCurrent_ToAngle_Redline_InRads) * size)
                                    };

                                    ImU32 col[2] {
                                       activeHUDColourPreset.RPMGauge_Redline_Foreground_Colour,
                                       activeHUDColourPreset.RPMGauge_Redline_Foreground_Colour_GradientEnd
                                    };
                                    draw_list->PathStrokeGradient(gradient_p, col, false, size * 0.1f);
                                 } else {
                                    draw_list->PathStroke(activeHUDColourPreset.RPMGauge_Redline_Foreground_Colour, false, size * 0.1f);
                                 }
                              }
                           }
                        }

                        // Needle
                        {
                           ImGui::ImRotateStart();
                           draw_list->AddTriangleFilled(
                              ImVec2(cursorPos.x + size * 1.25f, cursorPos.y + size * 0.95f),
                              ImVec2(cursorPos.x + size * 2.00f, cursorPos.y + size),
                              ImVec2(cursorPos.x + size * 1.25f, cursorPos.y + size * 1.05f),
                              activeHUDColourPreset.RPMGauge_Needle_Foreground_Colour
                           );
                           ImGui::ImRotateEnd(
                              rpmValueCurrent_ToAngle_FullRange_InRads,
                              ImVec2(cursorPos.x + size, cursorPos.y + size)
                           );
                        }
                     }

                     // Speed drawing
                     {
                        const GameInternals::SpeedUnit speedUnitType = GameInternals::Config::Gameplay::getSpeedUnitType();
                        const float                    speedValue    = PlayerCarInternals::getSpeed(speedUnitType);

                        static char speedValueBackgroundText[4];
                        static char speedValueToText[4];
                        static char speedUnitTypeToText[4];
                        sprintf_s(speedValueBackgroundText, "%03.0f", speedValue);
                        sprintf_s(speedValueToText, "%.0f", speedValue);
                        sprintf_s(speedUnitTypeToText, "%s",
                                  speedUnitType == GameInternals::SpeedUnit::KMH ? "KMH" :
                                  speedUnitType == GameInternals::SpeedUnit::MPH ? "MPH" :
                                  "MPS"
                        );

                        static ImVec2 speedTextDrawingSize;
                        static ImVec2 speedTextDrawingCursorPosition;

                        // Speed value
                        {
                           ImGui::PushFont(imguiIO->Fonts->Fonts[4]); // Big size Aramis Book Italic

                           // Background filler
                           {
                              speedTextDrawingSize = ImGui::CalcTextSize(speedValueBackgroundText);
                              speedTextDrawingCursorPosition = ImVec2(
                                 cursorPos.x + (size * 2.15f) - speedTextDrawingSize.x,
                                 cursorPos.y + (size * 1.35f)
                              );

                              ImGui::SetCursorScreenPos(speedTextDrawingCursorPosition);
                              ImGui::TextColored(activeHUDColourPreset.SpeedDisplay_FillerText_Colour, speedValueBackgroundText);
                           }
                           // Value
                           {
                              speedTextDrawingSize = ImGui::CalcTextSize(speedValueToText);
                              speedTextDrawingCursorPosition = ImVec2(
                                 cursorPos.x + (size * 2.15f) - speedTextDrawingSize.x,
                                 cursorPos.y + (size * 1.35f)
                              );

                              ImGui::SetCursorScreenPos(speedTextDrawingCursorPosition);
                              ImGui::TextColored(activeHUDColourPreset.SpeedDisplay_ValueText_Colour, speedValueToText);
                           }
                           ImGui::PopFont();
                        }
                        // Speed Unit
                        {
                           ImGui::PushFont(imguiIO->Fonts->Fonts[3]);

                           speedTextDrawingSize = ImGui::CalcTextSize(speedUnitTypeToText);
                           speedTextDrawingCursorPosition = ImVec2(
                              cursorPos.x + size * 2.20f - speedTextDrawingSize.x,
                              cursorPos.y + size * 1.15f
                           );

                           ImGui::SetCursorScreenPos(speedTextDrawingCursorPosition);
                           ImGui::TextColored(activeHUDColourPreset.SpeedDisplay_UnitTypeText_Colour, speedUnitTypeToText);

                           ImGui::PopFont();
                        }
                     }

                     // NOS drawing
                     {
                        static const float NOSBarRangeStartAngle = 136.2f;
                        static const float NOSBarEndAngle        = 366.1f;
                        static const int   nosMaximumSegments    = static_cast<int>(NOSBarEndAngle - NOSBarRangeStartAngle); // this needs to be an odd number

                        const float nosValue           = PlayerCarInternals::getNOSValue();
                        const int   nosCurrentSegments = static_cast<int>(std::round(nosValue * nosMaximumSegments));

                        // Empty bars
                        {
                           draw_list->PathArcToDashedAndStroke(
                              ImVec2(cursorPos.x + size, cursorPos.y + size),
                              size * 1.125f,
                              NOSBarRangeStartAngle * DegreeToRadianMultiplier,
                              NOSBarEndAngle        * DegreeToRadianMultiplier,
                              activeHUDColourPreset.NOSBar_Value_Background_Colour,
                              size * 0.08f,
                              nosMaximumSegments,
                              1,
                              1,
                              nosCurrentSegments,
                              nosMaximumSegments
                           );
                        }
                        // Full bars
                        {
                           draw_list->PathArcToDashedAndStroke(
                              ImVec2(cursorPos.x + size, cursorPos.y + size),
                              size * 1.125f,
                              NOSBarRangeStartAngle * DegreeToRadianMultiplier,
                              NOSBarEndAngle        * DegreeToRadianMultiplier,
                              activeHUDColourPreset.NOSBar_Value_Foreground_Colour,
                              size * 0.1f,
                              nosMaximumSegments,
                              1,
                              1,
                              0,
                              nosCurrentSegments
                           );
                        }
                     }
                  }
                  ImGui::End();
                  ImGui::PopStyleVar();
               }
            }
         }

         const virtual bool displayMenuItem(const ImVec2& buttonSize) override {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.59f, 0.26f, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.59f, 0.26f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.53f, 0.06f, 1.00f));
            bool ret = ImGui::Button("Custom HUD", buttonSize);
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            return ret;
         }
         const virtual bool displayMenu() override {
            ImGui::Checkbox("Enabled", &isEnabled);
            if (isEnabled) {
               presetControls.Draw();

               ImGui::Checkbox("Draw gradients", &activeHUDColourPreset.DrawGradients);
               ImGui::SameLine(); ImGui::Checkbox("Clear background colours", &activeHUDColourPreset.ClearBackgroundColours);
               ImGui::Checkbox("Let game draw gauges too", GameInternals::Config::Player::getIsShowGameGaugesAsPointer());

               //ImGui::ColorEdit4("RPMGauge_BackColor", &activeHUDColourPreset.RPMGauge_BackColour.Value.x);

               ImGui::ColorEdit4("RPMGauge_NormalRange_Background_Colour", &activeHUDColourPreset.RPMGauge_NormalRange_Background_Colour.Value.x);
               if (activeHUDColourPreset.DrawGradients)
                  ImGui::ColorEdit4("RPMGauge_NormalRange_Background_Colour_GradientEnd", &activeHUDColourPreset.RPMGauge_NormalRange_Background_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("RPMGauge_NormalRange_Foreground_Colour", &activeHUDColourPreset.RPMGauge_NormalRange_Foreground_Colour.Value.x);
               if (activeHUDColourPreset.DrawGradients)
                  ImGui::ColorEdit4("RPMGauge_NormalRange_Foreground_Colour_GradientEnd", &activeHUDColourPreset.RPMGauge_NormalRange_Foreground_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("RPMGauge_Redline_Background_Colour", &activeHUDColourPreset.RPMGauge_Redline_Background_Colour.Value.x);
               if (activeHUDColourPreset.DrawGradients)
                  ImGui::ColorEdit4("RPMGauge_Redline_Background_Colour_GradientEnd", &activeHUDColourPreset.RPMGauge_Redline_Background_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("RPMGauge_Redline_Foreground_Colour", &activeHUDColourPreset.RPMGauge_Redline_Foreground_Colour.Value.x);
               if (activeHUDColourPreset.DrawGradients)
                  ImGui::ColorEdit4("RPMGauge_Redline_Foreground_Colour_GradientEnd", &activeHUDColourPreset.RPMGauge_Redline_Foreground_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("RPMGauge_Needle_Foreground_Color", &activeHUDColourPreset.RPMGauge_Needle_Foreground_Colour.Value.x);
               ImGui::Separator();

               ImGui::ColorEdit4("SpeedDisplay_UnitTypeText_Color", &activeHUDColourPreset.SpeedDisplay_UnitTypeText_Colour.Value.x);
               ImGui::ColorEdit4("SpeedDisplay_FillerText_Color", &activeHUDColourPreset.SpeedDisplay_FillerText_Colour.Value.x);
               ImGui::ColorEdit4("SpeedDisplay_ValueText_Color", &activeHUDColourPreset.SpeedDisplay_ValueText_Colour.Value.x);
               ImGui::Separator();

               //ImGui::ColorEdit4("NOSBar_Background_Color", &activeHUDColourPreset.NOSBar_Background_Colour.Value.x);
               ImGui::ColorEdit4("NOSBar_Value_Background_Color", &activeHUDColourPreset.NOSBar_Value_Background_Colour.Value.x);
               ImGui::ColorEdit4("NOSBar_Value_Foreground_Color", &activeHUDColourPreset.NOSBar_Value_Foreground_Colour.Value.x);
            }
            return true;
         }
      };
   }
}