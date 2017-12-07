#include "stdafx.h"
#include "InGameMenu.h"
// Helpers
#include "Memory.h"
#include "Settings.h"
#include "WndProcHook.h"
#include "Game Internals\GameInternals.h"
// Drawing Helpers
#include "imgui\imgui.h"
#include "imgui_ext.h"
#include "imgui\extra_fonts\RobotoMedium.hpp"
#include "imgui\extra_fonts\Aramis_Book_Italic.hpp"

#include <d3d9.h>
#include MIRRORHOOK_DEFINITIONS_PATH

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "imgui\dx9\imgui_impl_dx9.h"

//SetWindowSubClass
#include <commctrl.h>

#include <map>
using std::map;

namespace Extensions {
   namespace InGameMenu {
      enum class MenuOption {
         CustomCamera,
         GameplayOptions,
         NewHUD
      };

      namespace State {
         map<MenuOption, bool> enabledOptions =
         {
            { MenuOption::NewHUD,          false }
         };
         map<MenuOption, bool> visibleMenus =
         {
            { MenuOption::CustomCamera,    false },
            { MenuOption::GameplayOptions, false },
            { MenuOption::NewHUD,          false }
         };
      }

      bool isImguiInitialized  = false;
      bool isMainWindowVisible = true;
      ImGuiIO* imguiIO         = nullptr;
      bool isFirstTimeUser     = false;

      // Game variables
      GameInternals::CarPowerData* carPowerData = nullptr;

      void WINAPI beginScene(LPDIRECT3DDEVICE9 pDevice) {
         if (!isImguiInitialized) {
            ImGui_ImplDX9_Init(Hooks::WndProc::windowHandle, pDevice);

            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = NULL;
            io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
            io.Fonts->AddFontFromMemoryCompressedTTF(Aramis_Book_Italic::Aramis_Book_Italic_compressed_data, Aramis_Book_Italic::Aramis_Book_Italic_compressed_size, 16.0f);
            io.Fonts->AddFontFromMemoryCompressedTTF(Aramis_Book_Italic::Aramis_Book_Italic_compressed_data, Aramis_Book_Italic::Aramis_Book_Italic_compressed_size, 80.0f);
            io.FontDefault = NULL;

            ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview
                                       | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_PickerHueWheel);
            ImGui::LoadStyle();
            imguiIO = &ImGui::GetIO();
            isImguiInitialized = true;
         }

         ImGui_ImplDX9_NewFrame();
      }

      void showUserGuide() {
         if (isFirstTimeUser) {
            ImGui::Begin("User Guide", &isFirstTimeUser);
            ImGui::BulletText("Press Insert to show/hide RockportEd.");
            ImGui::BulletText("Double-click on title bar to collapse windows.");
            ImGui::BulletText("Click and drag on lower right corner to resize windows.");
            ImGui::BulletText("Click and drag on any empty space to move windows.");
            ImGui::BulletText("Mouse Wheel to scroll.");
            ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            ImGui::BulletText("CTRL+Click on a slider to input text.");
            ImGui::BulletText(
               "While editing text:\n"
               "- Hold SHIFT+Left/Right or use mouse to select text\n"
               "- CTRL+Left/Right to word jump\n"
               "- CTRL+A or double-click to select all\n"
               "- CTRL+X, CTRL+C, CTRL+V clipboard\n"
               "- CTRL+Z, CTRL+Y to undo/redo\n"
               "- ESCAPE to cancel\n"
               "- You can apply arithmetic operators +,*,/ on numerical values.\n");
            ImGui::BulletText("Click on the button at the top-right of this window to close it.");
            ImGui::End();

            Settings::settingsType.isFirstTimeUser = false;
            Settings::saveSettings();
         }
      }
      void customCamera() {
         ImGui::Checkbox("Camera Settings", &State::visibleMenus[MenuOption::CustomCamera]);
         if (State::visibleMenus[MenuOption::CustomCamera]) {
            ImGui::SetNextWindowSize(ImVec2(240.0f, 400.0f), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(10.0f * 2 + 200.0f, 5.0f), ImGuiCond_Once);
            ImGui::Begin("Custom Camera", &State::visibleMenus[MenuOption::CustomCamera]);

            ImGui::PushItemWidth(-1);

            static int* activeCameraIndex = nullptr;
            activeCameraIndex = GameInternals::Gameplay::Camera::getActiveCameraIndexAsPointer();
            if (activeCameraIndex) {
               ImGui::TextWrapped("Camera Index"); ImGui::SameLine();
               ImGui::SliderInt("##CameraIndex", activeCameraIndex, 0, 6);

               static GameInternals::CameraInfo* activeCameraData = nullptr;
               if (GameInternals::Gameplay::Camera::getActiveCameraInfo(activeCameraData)) {
                  ImGui::TextWrapped("Camera Name: %s", activeCameraData->collectionName);

                  static bool isAdvanced = false;
                  ImGui::Checkbox("Display advanced options", &isAdvanced);

                  ImGui::TextWrapped("Stiffness");
                  ImGui::SliderFloat("##CameraStiffness", &activeCameraData->stiffness[0], 0.0f, 1.0f);
                  if (isAdvanced) {
                     ImGui::Text("STIFFNESS[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraStiffness_1", &activeCameraData->stiffness[1], 0.0f, 1.0f);
                  }

                  ImGui::TextWrapped("Horizontal angle");
                  ImGui::SliderFloat("##CameraHorizontalAngle", &activeCameraData->angle[0], -45.0f, 45.0f, "%.3f deg");
                  if (isAdvanced) {
                     ImGui::Text("ANGLE[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraHorizontalAngle_1", &activeCameraData->angle[1], -45.0f, 45.0f, "%.3f deg");
                  }

                  ImGui::TextWrapped("Distance");
                  ImGui::SliderFloat("##CameraDistance", &activeCameraData->lag[0], -100.0f, 100.0f);
                  if (isAdvanced) {
                     ImGui::Text("LAG[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraDistance_1", &activeCameraData->lag[1], -100.0f, 100.0f);
                  }

                  ImGui::TextWrapped("FOV");
                  ImGui::SliderFloat("##CameraFOV", &activeCameraData->fov[0], 25.0f, 135.0f);
                  if (isAdvanced) {
                     ImGui::Text("FOV[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraFOV_1", &activeCameraData->fov[1], 25.0f, 135.0f);
                  }

                  ImGui::TextWrapped("Height");
                  ImGui::SliderFloat("##CameraHeight", &activeCameraData->height[0], 0.0f, 100.0f);
                  if (isAdvanced) {
                     ImGui::Text("HEIGHT[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraHeight_1", &activeCameraData->height[1], 0.0f, 100.0f);
                  }

                  ImGui::TextWrapped("Vertical angle");
                  ImGui::SliderFloat("##CameraLateOffset", &activeCameraData->lateOffset[0], -89.0f, 89.0f, "%.3f deg");
                  if (isAdvanced) {
                     ImGui::Text("LATEOFFSET[1]"); ImGui::SameLine();
                     ImGui::SliderFloat("##CameraLateOffset_1", &activeCameraData->lateOffset[1], -89.0f, 89.0f);
                  }

                  ImGui::Checkbox("Tilting", &activeCameraData->tilting[0]);
                  if (isAdvanced) {
                     ImGui::Checkbox("TILTING[1]", &activeCameraData->tilting[1]);
                  }

                  ImGui::Checkbox("Selectable", &activeCameraData->selectable[0]);
                  if (isAdvanced) {
                     ImGui::Checkbox("SELECTABLE[1]", &activeCameraData->selectable[1]);
                  }
               }
               else {
                  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  ImGui::TextWrapped("CameraInfo not yet initialized by the game.");
                  ImGui::PopStyleColor();
               }
            }
            else {
               ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
               ImGui::TextWrapped("CameraAI not yet initialized by the game.");
               ImGui::PopStyleColor();
            }
            ImGui::PopItemWidth();
            ImGui::End();
         }
      }
      void gameplayOptions() {
         ImGui::Checkbox("Gameplay Options", &State::visibleMenus[MenuOption::GameplayOptions]);
         if (State::visibleMenus[MenuOption::GameplayOptions]) {
            ImGui::SetNextWindowSize(ImVec2(0.0f, 160.0f), ImGuiCond_Once);
            ImGui::SetNextWindowPos(ImVec2(10.0f, 5.0f * 2 + 120.0f), ImGuiCond_Once);
            ImGui::Begin("Gameplay Options", &State::visibleMenus[MenuOption::GameplayOptions]);

            ImGui::PushItemWidth(-1);

            // Traffic refresh interval
            {
               ImGui::TextWrapped("Traffic refresh interval (in seconds)");
               static float* trafficRefreshInterval = nullptr;

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

            ImGui::Separator();

            // Allow to redline RPM
            // TODO: Increase car's max rpm instead of the general RPM multiplier
            // TODO: This function works on checkbox interaction, which means it doesn't update itself. Fix that
            {
               static bool allowToRedlineRPM = false;
               if (!carPowerData)
                  ImGui::PushItemDisabled();

               if (ImGui::Checkbox("Allow to redline RPM", &allowToRedlineRPM)) {
                  float newVal = allowToRedlineRPM ?
                     ((10000.0f / carPowerData->maximumRPM) * carPowerData->maximumPossibleRPM) - 1000.0f
                     : 9000.0f;
                  GameInternals::Gameplay::Variables::setSomeRpmConstant(newVal);
               }

               if (!carPowerData) {
                  ImGui::PopItemDisabled();
                  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                  ImGui::TextWrapped("Car power data is not yet found in game's memory.");
                  ImGui::PopStyleColor();
               }
            }

            ImGui::PopItemWidth();
            ImGui::End();
         }
      }

      void WINAPI endScene(LPDIRECT3DDEVICE9 pDevice) {
         if (isImguiInitialized) {
            ImGui::SetNextWindowPos(ImVec2(imguiIO->DisplaySize.x - 5.0f, 5.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            ImGui::Begin("##Huh", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "RockportEd debug build");
            ImGui::Bullet(); ImGui::TextWrapped("Allow to redline rpm should be disabled when entering gameplay. Only enable it during gameplay.");
            ImGui::Bullet(); ImGui::TextWrapped("Menu input <-> game input handling is broken.");
            ImGui::End();

            // TODO: Optmize new hud code, structuer it blabla
            GameInternals::Gameplay::Player::getCarPowerData(carPowerData);

            if (isMainWindowVisible) {
               imguiIO->MouseDrawCursor = imguiIO->WantCaptureMouse;

               showUserGuide();

               ImGui::SetNextWindowPos(ImVec2(10.0f, 5.0f), ImGuiCond_Once);
               ImGui::Begin("RockportEd", nullptr);

               customCamera();
               gameplayOptions();
               ImGui::Checkbox("New HUD", &State::enabledOptions[MenuOption::NewHUD]);

               ImGui::End();
               /*
               ImGui::Begin("_DEBUG-2", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
               static float newRideHeight = 0.0f;
               static PhyicsTuning abc = { 0 };
               static DWORD addr_abc = (DWORD)&abc;

               if (ImGui::SliderFloat("NewRideHeight", &newRideHeight, -6.0f, 12.0f)) {
                  static GameInternals::ObjectData* playerData = nullptr;
                  if (GameInternals::Gameplay::Object::getObjectData(playerData)) {
                     if (newRideHeight > abc.rideheight && !playerData->z_Velocity)
                        playerData->z_Position += 0.005f;
                  }
                  abc.rideheight = newRideHeight;
                  __asm {
                     push eax
                     push ebx
                     push ecx
                     push edx

                     push 0x10

                     mov edx, 0x227400 + 0x400000 // Event::__nw(uint)
                     call edx
                     add esp, 0x4

                     //mov ebx, [eax - 0x4]
                     mov ebx, [addr_abc]
                     push ebx // arg 2
                     push 0x0   // arg 1
                     mov ecx, eax // inner function variable

                     mov edx, 0x225840 + 0x400000 // ETuneVehicle::ETuneVehicle(uint,Physics::Tunings const *)
                     call edx

                     pop edx
                     pop ecx
                     pop ebx
                     pop eax
                  }
               }

               ImGui::End();*/
            }
            else {
               imguiIO->MouseDrawCursor = false;
            }

            if (State::enabledOptions[MenuOption::NewHUD] && carPowerData) {
               using namespace GameInternals::Gameplay::Player;

               ImGui::Begin("New HUD Color Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
               static bool useGradients = false;
               static bool drawForegroundOnBackground = true;
               ImGui::Checkbox("Use gradients", &useGradients);
               ImGui::SameLine(); ImGui::Checkbox("Draw foregrounds on top of the backgrounds", &drawForegroundOnBackground);

               ImGui::ColorEdit4("rpmGauge_BackColor", &Settings::settingsType.colours.hudColours.rpmGauge_BackColour.Value.x);

               ImGui::ColorEdit4("rpmGauge_NormalRange_Background_Color1", &Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Background_Colour.Value.x);
               if (useGradients)
                  ImGui::ColorEdit4("rpmGauge_NormalRange_Background_Colour_GradientEnd", &Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Background_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("rpmGauge_NormalRange_Foreground_Colour", &Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Foreground_Colour.Value.x);
               if (useGradients)
                  ImGui::ColorEdit4("rpmGauge_NormalRange_Foreground_Colour_GradientEnd", &Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Foreground_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("rpmGauge_Redline_Background_Colour", &Settings::settingsType.colours.hudColours.rpmGauge_Redline_Background_Colour.Value.x);
               if (useGradients)
                  ImGui::ColorEdit4("rpmGauge_Redline_Background_Colour_GradientEnd", &Settings::settingsType.colours.hudColours.rpmGauge_Redline_Background_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("rpmGauge_Redline_Foreground_Colour", &Settings::settingsType.colours.hudColours.rpmGauge_Redline_Foreground_Colour.Value.x);
               if (useGradients)
                  ImGui::ColorEdit4("rpmGauge_Redline_Foreground_Colour_GradientEnd", &Settings::settingsType.colours.hudColours.rpmGauge_Redline_Foreground_Colour_GradientEnd.Value.x);

               ImGui::ColorEdit4("rpmGauge_Needle_Foreground_Color", &Settings::settingsType.colours.hudColours.rpmGauge_Needle_Foreground_Colour.Value.x);
               ImGui::Separator();

               ImGui::ColorEdit4("speedDisplay_UnitTypeText_Color", &Settings::settingsType.colours.hudColours.speedDisplay_UnitTypeText_Colour.Value.x);
               ImGui::ColorEdit4("speedDisplay_FillerText_Color", &Settings::settingsType.colours.hudColours.speedDisplay_FillerText_Colour.Value.x);
               ImGui::ColorEdit4("speedDisplay_ValueText_Color", &Settings::settingsType.colours.hudColours.speedDisplay_ValueText_Colour.Value.x);
               ImGui::Separator();

               ImGui::ColorEdit4("nosBar_Background_Color", &Settings::settingsType.colours.hudColours.nosBar_Background_Colour.Value.x);
               ImGui::ColorEdit4("nosBar_Value_Background_Color", &Settings::settingsType.colours.hudColours.nosBar_Value_Background_Colour.Value.x);
               ImGui::ColorEdit4("nosBar_Value_Foreground_Color", &Settings::settingsType.colours.hudColours.nosBar_Value_Foreground_Colour.Value.x);

               ImGui::End();

               //GameInternals::Config::Player::setManualTransmissionEnabled(true);

               const float sz = 1.0f * 90.0f;
               //ImGui::SetNextWindowSize(ImVec2(sz * 3.0f, sz * 3.0f), ImGuiCond_Once);
               ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4());
               ImGui::Begin("##HUD", nullptr, ImGuiWindowFlags_NoTitleBar);

            #define DegreeToRadianMultiplier 0.0174532925f

               // Drawing variables
               ImDrawList* draw_list = ImGui::GetWindowDrawList();
               ImVec2      cursorPos = ImGui::GetCursorScreenPos();
               cursorPos.x += sz / 5.0f; // nos padding
               cursorPos.y += sz / 5.0f; // nos padding

               // RPM drawing
               {
                  static const float rpmValueMaximum             = 9000.0f;
                  //static const float rpmValueRedlineOffset       = 450.0f; not certain whether to use this
                  static const float rpmBeginningAngle_FullRange = 135.0f;
                  static const float rpmFinalAngle_FullRange     = 365.0f;
                  static const float rpmAngleDiff_BeginningToFinal_FullRange = rpmFinalAngle_FullRange - rpmBeginningAngle_FullRange;

                  const float rpmValueCurrent = getCarRPM(carPowerData);

                  const float rpmBeginningValue_Redline             = carPowerData->maximumRPM;
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
                  const float rpmFinalAngle_FullRange_InRads             = rpmFinalAngle_FullRange     * DegreeToRadianMultiplier;
                  const float rpmBeginningAngle_Redline_InRads           = rpmBeginningAngle_Redline   * DegreeToRadianMultiplier;
                  const float rpmValueCurrent_ToAngle_FullRange_InRads   = rpmValueCurrent_ToAngle_FullRange   * DegreeToRadianMultiplier;
                  const float rpmValueCurrent_ToAngle_NormalRange_InRads = rpmValueCurrent_ToAngle_NormalRange * DegreeToRadianMultiplier;
                  const float rpmValueCurrent_ToAngle_Redline_InRads     = rpmValueCurrent_ToAngle_Redline     * DegreeToRadianMultiplier;

                  // Gauge
                  {
                     // RPM drawing (Normal Range)
                     {
                        // Background
                        {
                           draw_list->PathArcTo(
                              ImVec2(cursorPos.x + sz, cursorPos.y + sz),
                              sz,
                              drawForegroundOnBackground ? rpmBeginningAngle_FullRange_InRads : rpmValueCurrent_ToAngle_NormalRange_InRads,
                              rpmBeginningAngle_Redline_InRads,
                              32
                           );

                           if (useGradients) {
                              ImVec2 gradient_p[2] {
                                 ImVec2(
                                    cursorPos.x + sz + cosf(rpmBeginningAngle_FullRange_InRads) * sz,
                                    cursorPos.y + sz + sinf(rpmBeginningAngle_FullRange_InRads) * sz),
                                 ImVec2(
                                    cursorPos.x + sz + cosf(rpmBeginningAngle_Redline_InRads) * sz,
                                    cursorPos.y + sz + sinf(rpmBeginningAngle_Redline_InRads) * sz)
                              };

                              ImU32 col[2] {
                                 Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Background_Colour,
                                 Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Background_Colour_GradientEnd
                              };
                              draw_list->PathStrokeGradient(gradient_p, col, false, sz * 0.1f);
                           }
                           else {
                              draw_list->PathStroke(Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Background_Colour, false, sz * 0.1f);
                           }
                        }

                        // Foreground
                        {
                           draw_list->PathArcTo(
                              ImVec2(cursorPos.x + sz, cursorPos.y + sz),
                              sz,
                              rpmBeginningAngle_FullRange_InRads,
                              rpmValueCurrent_ToAngle_NormalRange_InRads,
                              32
                           );

                           if (useGradients) {
                              ImVec2 gradient_p[2] {
                                 ImVec2(
                                 cursorPos.x + sz + cosf(rpmBeginningAngle_FullRange_InRads) * sz,
                                 cursorPos.y + sz + sinf(rpmBeginningAngle_FullRange_InRads) * sz),
                              ImVec2(
                                 cursorPos.x + sz + cosf(rpmValueCurrent_ToAngle_NormalRange_InRads) * sz,
                                 cursorPos.y + sz + sinf(rpmValueCurrent_ToAngle_NormalRange_InRads) * sz)
                              };

                              ImU32 col[2] {
                                 Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Foreground_Colour,
                                 Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Foreground_Colour_GradientEnd
                              };
                              draw_list->PathStrokeGradient(gradient_p, col, false, sz * 0.1f);
                           }
                           else {
                              draw_list->PathStroke(Settings::settingsType.colours.hudColours.rpmGauge_NormalRange_Foreground_Colour, false, sz * 0.1f);
                           }
                        }
                     }
                     // RPM drawing (Redline)
                     {
                        // Background
                        {
                           draw_list->PathArcTo(
                              ImVec2(cursorPos.x + sz, cursorPos.y + sz),
                              sz,
                              drawForegroundOnBackground ? rpmBeginningAngle_Redline_InRads : rpmValueCurrent_ToAngle_Redline_InRads,
                              rpmFinalAngle_FullRange_InRads,
                              32
                           );

                           if (useGradients) {
                              ImVec2 gradient_p[2] {
                                 ImVec2(
                                    cursorPos.x + sz + cosf(rpmBeginningAngle_Redline_InRads) * sz,
                                    cursorPos.y + sz + sinf(rpmBeginningAngle_Redline_InRads) * sz),
                                 ImVec2(
                                    cursorPos.x + sz + cosf(rpmFinalAngle_FullRange_InRads) * sz,
                                    cursorPos.y + sz + sinf(rpmFinalAngle_FullRange_InRads) * sz)
                              };

                              ImU32 col[2] {
                                 Settings::settingsType.colours.hudColours.rpmGauge_Redline_Background_Colour,
                                 Settings::settingsType.colours.hudColours.rpmGauge_Redline_Background_Colour_GradientEnd
                              };
                              draw_list->PathStrokeGradient(gradient_p, col, false, sz * 0.1f);
                           }
                           else {
                              draw_list->PathStroke(Settings::settingsType.colours.hudColours.rpmGauge_Redline_Background_Colour, false, sz * 0.1f);
                           }
                        }
                        // Foreground
                        {
                           draw_list->PathArcTo(
                              ImVec2(cursorPos.x + sz, cursorPos.y + sz),
                              sz,
                              rpmBeginningAngle_Redline_InRads,
                              rpmValueCurrent_ToAngle_Redline_InRads,
                              32
                           );

                           if (useGradients) {
                              ImVec2 gradient_p[2] {
                                 ImVec2(
                                    cursorPos.x + sz + cosf(rpmBeginningAngle_Redline_InRads) * sz,
                                    cursorPos.y + sz + sinf(rpmBeginningAngle_Redline_InRads) * sz),
                                 ImVec2(
                                    cursorPos.x + sz + cosf(rpmValueCurrent_ToAngle_Redline_InRads) * sz,
                                    cursorPos.y + sz + sinf(rpmValueCurrent_ToAngle_Redline_InRads) * sz)
                              };

                              ImU32 col[2] {
                                 Settings::settingsType.colours.hudColours.rpmGauge_Redline_Foreground_Colour,
                                 Settings::settingsType.colours.hudColours.rpmGauge_Redline_Foreground_Colour_GradientEnd
                              };
                              draw_list->PathStrokeGradient(gradient_p, col, false, sz * 0.1f);
                           }
                           else {
                              draw_list->PathStroke(Settings::settingsType.colours.hudColours.rpmGauge_Redline_Foreground_Colour, false, sz * 0.1f);
                           }
                        }
                     }
                  }
                  // Needle
                  {
                     ImRotateStart();
                     draw_list->AddTriangleFilled(
                        ImVec2(cursorPos.x + sz, cursorPos.y + sz * 0.95f),
                        ImVec2(cursorPos.x + sz * 2.0f, cursorPos.y + sz),
                        ImVec2(cursorPos.x + sz, cursorPos.y + sz * 1.05f),
                        Settings::settingsType.colours.hudColours.rpmGauge_Needle_Foreground_Colour
                     );
                     ImRotateEnd(
                        rpmValueCurrent_ToAngle_FullRange_InRads,
                        ImVec2(cursorPos.x + sz, cursorPos.y + sz)
                     );
                  }
               }

               // Speed drawing
               {
                  const GameInternals::SpeedUnit speedUnitType = GameInternals::Config::Gameplay::getSpeedUnitType();
                  const float                    speedValue    = getCarSpeed(speedUnitType);

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
                     ImGui::PushFont(imguiIO->Fonts->Fonts[2]); // 80.0f Aramis Book Italic

                     // Background filler
                     {
                        speedTextDrawingSize = ImGui::CalcTextSize(speedValueBackgroundText);
                        speedTextDrawingCursorPosition = ImVec2(
                           cursorPos.x + (sz * 2.15f) - speedTextDrawingSize.x,
                           cursorPos.y + (sz * 1.35f)
                        );

                        ImGui::SetCursorScreenPos(speedTextDrawingCursorPosition);
                        ImGui::TextColored(Settings::settingsType.colours.hudColours.speedDisplay_FillerText_Colour, speedValueBackgroundText);
                     }
                     // Value
                     {
                        speedTextDrawingSize = ImGui::CalcTextSize(speedValueToText);
                        speedTextDrawingCursorPosition = ImVec2(
                           cursorPos.x + (sz * 2.15f) - speedTextDrawingSize.x,
                           cursorPos.y + (sz * 1.35f)
                        );

                        ImGui::SetCursorScreenPos(speedTextDrawingCursorPosition);
                        ImGui::TextColored(Settings::settingsType.colours.hudColours.speedDisplay_ValueText_Colour, speedValueToText);
                     }
                     ImGui::PopFont();
                  }
                  // Speed Unit
                  {
                     ImGui::PushFont(imguiIO->Fonts->Fonts[1]);

                     speedTextDrawingSize = ImGui::CalcTextSize(speedUnitTypeToText);
                     speedTextDrawingCursorPosition = ImVec2(
                        cursorPos.x + sz * 2.20f - speedTextDrawingSize.x,
                        cursorPos.y + sz * 1.15f
                     );

                     ImGui::SetCursorScreenPos(speedTextDrawingCursorPosition);
                     ImGui::TextColored(Settings::settingsType.colours.hudColours.speedDisplay_UnitTypeText_Colour, speedUnitTypeToText);

                     ImGui::PopFont();
                  }
               }

               // NOS drawing
               {
                  static const float nosBarRangeStartAngle = 136.2f;
                  static const float nosBarEndAngle        = 366.1f;
                  static const int   nosMaximumSegments    = static_cast<int>(nosBarEndAngle - nosBarRangeStartAngle); // this needs to be an odd number

                  const float nosValue           = getCarNOS();
                  const int   nosCurrentSegments = static_cast<int>(std::round(nosValue * nosMaximumSegments));

                  // Empty bars
                  {
                     draw_list->PathArcToDashedAndStroke(
                        ImVec2(cursorPos.x + sz, cursorPos.y + sz),
                        sz * 1.125f,
                        nosBarRangeStartAngle * DegreeToRadianMultiplier,
                        nosBarEndAngle        * DegreeToRadianMultiplier,
                        Settings::settingsType.colours.hudColours.nosBar_Value_Background_Colour,
                        sz * 0.08f,
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
                        ImVec2(cursorPos.x + sz, cursorPos.y + sz),
                        sz * 1.125f,
                        nosBarRangeStartAngle * DegreeToRadianMultiplier,
                        nosBarEndAngle        * DegreeToRadianMultiplier,
                        Settings::settingsType.colours.hudColours.nosBar_Value_Foreground_Colour,
                        sz * 0.1f,
                        nosMaximumSegments,
                        1,
                        1,
                        0,
                        nosCurrentSegments
                     );
                  }
               }

               ImGui::End();
               ImGui::PopStyleColor();
            }
            ImGui::Render();
         }
      }

      void WINAPI beforeReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
         if (isImguiInitialized)
            ImGui_ImplDX9_InvalidateDeviceObjects();
      }
      void WINAPI afterReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
         if (!pDevice || pDevice->TestCooperativeLevel() != D3D_OK) {
            ImGui_ImplDX9_Shutdown();
            isImguiInitialized = false;
         }
         else {
            pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
         }

         ImGui_ImplDX9_CreateDeviceObjects();
      }

      LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
         if (isImguiInitialized) {
            if (!(imguiIO->WantTextInput | imguiIO->WantCaptureKeyboard)) {
               if (uMsg == WM_KEYUP) {
                  switch (wParam) {
                     case VK_INSERT:
                        isMainWindowVisible = !isMainWindowVisible;
                        break;
                  }
               }
            }

            if (isMainWindowVisible)
               ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
         }
         return FALSE;
      }

      DWORD WINAPI Init(LPVOID) {
         HANDLE hMirrorHook          = nullptr;
         LPDIRECT3DDEVICE9 d3dDevice = nullptr;
         while (!d3dDevice) {
            if (!hMirrorHook) {
               hMirrorHook = GetModuleHandle("MirrorHook.asi");
            }
            else {
               d3dDevice = MirrorHook::D3D9::GetD3D9Device();
            }
            Sleep(100);
         }
         d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::BeginScene, &beginScene);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::EndScene, &endScene);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::BeforeReset, &beforeReset);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::AfterReset, &afterReset);

         Hooks::WndProc::addExtension(&wndProcExtension);

         isFirstTimeUser = Settings::settingsType.isFirstTimeUser;
         return TRUE;
      }
   }
}