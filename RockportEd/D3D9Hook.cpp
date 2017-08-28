#include "stdafx.h"
#include "D3D9Hook.h"
#include "VTableHook.hpp"
#include "Settings.h"
#include "D3D9Hook_Settings.h"
#include "Memory.h"
#include "Mods.h"

#include <d3d9.h>
#include "D3D9Types.h"

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

#include "imgui\imgui.h"
#include "imguiEx.h"
#include "imgui\extra_fonts\RobotoMedium.hpp"
#include "imgui\extra_fonts\Digital.hpp"
#include "imgui\extra_fonts\CooperHewitt_Italic.hpp"

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "imgui\dx9\imgui_impl_dx9.h"

#include <memory>
using std::unique_ptr;
using std::make_unique;

//SetWindowSubClass
#include <commctrl.h>

namespace D3D9Hook {
   HWND                   windowHandle  = nullptr;
   WNDPROC                origWndProc   = nullptr;
   LPDIRECT3DDEVICE9      d3dDevice     = nullptr;
   unique_ptr<VTableHook> d3dDeviceHook = nullptr;

   Reset_t           origReset           = nullptr;
   BeginScene_t      origBeginScene      = nullptr;
   EndScene_t        origEndScene        = nullptr;
   BeginStateBlock_t origBeginStateBlock = nullptr;

   bool showUserGuide = false;
   char* cameras[7] ={
      "Bumper",
      "Hood",
      "Near",
      "Far",
      "Challenge Entry",
      "Speedbreaker",
      "Pullback"
   };

   void doImGuiStyle() {
      ImGuiStyle* style = &ImGui::GetStyle();

      style->WindowMinSize       = ImVec2(10.0f, 10.0f);
      style->WindowPadding       = ImVec2(10.0f, 10.0f);
      style->WindowRounding      = 5.0f;
      style->ChildWindowRounding = 5.0f;
      style->FramePadding        = ImVec2(5.0f, 4.0f);
      style->FrameRounding       = 5.0f;
      style->ItemSpacing         = ImVec2(5.0f, 5.0f);
      style->ItemInnerSpacing    = ImVec2(10.0f, 10.0f);
      style->IndentSpacing       = 15.0f;
      style->ScrollbarSize       = 16.0f;
      style->ScrollbarRounding   = 5.0f;
      style->GrabMinSize         = 7.0f;
      style->GrabRounding        = 2.0f;

      style->Colors[ImGuiCol_Text]                 = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
      style->Colors[ImGuiCol_WindowBg]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_ChildWindowBg]        = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
      style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);
      style->Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
      style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
      style->Colors[ImGuiCol_FrameBg]              = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
      style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
      style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
      style->Colors[ImGuiCol_TitleBg]              = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
      style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.78f, 0.78f, 0.78f, 0.39f);
      style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
      style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
      style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
      style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
      style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
      style->Colors[ImGuiCol_ComboBg]              = ImVec4(0.78f, 0.78f, 0.78f, 0.98f);
      style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
      style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
      style->Colors[ImGuiCol_Button]               = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
      style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
      style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
      style->Colors[ImGuiCol_Header]               = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
      style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.20f, 0.51f, 0.67f, 1.00f);
      style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
      style->Colors[ImGuiCol_Separator]            = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
      style->Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
      style->Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.27f, 0.59f, 0.75f, 0.78f);
      style->Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.08f, 0.39f, 0.55f, 0.78f);
      style->Colors[ImGuiCol_CloseButton]          = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
      style->Colors[ImGuiCol_CloseButtonHovered]   = ImVec4(0.71f, 0.71f, 0.71f, 0.60f);
      style->Colors[ImGuiCol_CloseButtonActive]    = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
      style->Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
   }

   HRESULT WINAPI hkBeginScene(LPDIRECT3DDEVICE9 pDevice) {
      if (!D3D9HookSettings::isImguiInitialized) {
         ImGui_ImplDX9_Init(windowHandle, d3dDevice);

         ImGuiIO& io = ImGui::GetIO();
         io.IniFilename = NULL;
         io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
         io.Fonts->AddFontFromMemoryCompressedTTF(Digital::Digital_compressed_data, Digital::Digital_compressed_size, 34.0f);
         io.Fonts->AddFontFromMemoryCompressedTTF(CooperHewitt_Italic_compressed_data, CooperHewitt_Italic_compressed_size, 64.0f);
         io.FontDefault = NULL;

         doImGuiStyle();
         D3D9HookSettings::isImguiInitialized = true;
      }

      ImGui_ImplDX9_NewFrame();

      return origBeginScene(pDevice);
   }

   float resWidth, resHeight;
   float baseResWidth, baseResHeight;
   int gameResolutionCave() {
      int currentResIndex = *(int*)Memory::makeAbsolute(0x50181C);
      if (currentResIndex < 5) {
         DWORD* newResolutionSetupAddrs;
         newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2870);
         resWidth                = (float)*(int*)(newResolutionSetupAddrs[currentResIndex] + 0x0A);
         resHeight               = (float)*(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10);

         int ratio = (int)((resWidth / resHeight) * 100);
         if (ratio == 177) { // 16:9
            baseResWidth  = 850.0f;
            baseResHeight = 480.0f;
         }
         else if (ratio == 133) { // 4:3
            baseResWidth  = 640.0f - 3;
            baseResHeight = 480.0f - 5;
         }
      }
      return currentResIndex;
   }

   HRESULT WINAPI hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
      if (pDevice->TestCooperativeLevel() == D3D_OK) {
         if (D3D9HookSettings::isImguiInitialized) {
            ImGuiIO& io = ImGui::GetIO();

            if (D3D9HookSettings::Options::isMainWindowVisible) {
               io.MousePos.x      = (float)(*(LONG*)Memory::makeAbsolute(0x51CFB0)) * ((float)resWidth / baseResWidth);
               io.MousePos.y      = (float)(*(LONG*)Memory::makeAbsolute(0x51CFB4)) * ((float)resHeight / baseResHeight);
               io.MouseDrawCursor = io.WantCaptureMouse;

               if (showUserGuide) {
                  ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
                  ImGui::Begin("User Guide", &showUserGuide, ImVec2(0.0f, 0.0f), 1.0f,
                               ImGuiWindowFlags_NoSavedSettings);
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
               }
               if (D3D9HookSettings::Options::opt_CustomCamera) {
                  ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
                  ImGui::Begin("Custom Camera", &D3D9HookSettings::Options::opt_CustomCamera, ImVec2(200.0f, 200.0f), 0.8f);
                  ImGui::PushItemWidth(-1);

                  ImGui::TextWrapped("Camera");
                  ImGui::Combo("##CameraCombo", Mods::Camera::activeCamera, cameras, 7);

                  int activeCam = *Mods::Camera::activeCamera;
                  if (activeCam == 0 || (activeCam > 1 && activeCam < 4)) {
                     ImGui::TextWrapped("X");
                     ImGui::SliderFloat("##CameraX", Mods::Camera::data[activeCam]["X"], -50.0f, 50.0f);
                     ImGui::TextWrapped("Z");
                     ImGui::SliderFloat("##CameraZ", Mods::Camera::data[activeCam]["Z"], -50.0f, 50.0f);
                     ImGui::TextWrapped("FOV");
                     ImGui::SliderFloat("##CameraFov", Mods::Camera::data[activeCam]["Fov"], 25.0f, 120.0f);
                     ImGui::TextWrapped("Horizontal Angle");
                     ImGui::SliderFloat("##CameraHorAngle", Mods::Camera::data[activeCam]["HorAngle"], -45.0f, 45.0f, "%.3f deg");
                     ImGui::TextWrapped("Vertical Angle");
                     ImGui::SliderFloat("##CameraVerAngle", Mods::Camera::data[activeCam]["VerAngle"], -25.0f, 45.0f, "%.3f deg");
                  }

                  ImGui::PopItemWidth();
                  ImGui::End();
               }
               if (D3D9HookSettings::Options::opt_ReplayMenu) {
                  ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
                  ImGui::Begin("Replay System", &D3D9HookSettings::Options::opt_ReplayMenu, ImVec2(160.0f, 180.0f), 0.8f);

                  if (ImGui::Checkbox("Record", &Mods::ReplaySystem::Record::isRecording)) {
                     if (Mods::ReplaySystem::Record::isRecording)
                        Mods::ReplaySystem::Record::startRecording();
                  }
                  ImGui::Text("Frame Count: %u", Mods::ReplaySystem::Record::frameCount);

                  ImGui::Separator();

                  if (ImGui::Checkbox("Replay", &Mods::ReplaySystem::Replay::isShowingReplay)) {
                     if (Mods::ReplaySystem::Replay::isShowingReplay)
                        Mods::ReplaySystem::Replay::startReplay();
                  }
                  if (Mods::ReplaySystem::Replay::isShowingReplay) {
                     if (ImGui::Checkbox("Pause", &Mods::ReplaySystem::Replay::isReplayPaused)) {
                        Mods::ReplaySystem::Replay::changeReplayState();
                     }
                     ImGui::SliderUInt("Current Frame", &Mods::ReplaySystem::Replay::frameNr, 0, Mods::ReplaySystem::Record::frameCount - 1);
                  }
                  ImGui::End();
               }

               ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiCond_Once);
               ImGui::Begin("RockportEd", (bool*)0, ImVec2(200.0f, 140.0f), 0.9f);

               ImGui::Checkbox("Camera Settings", &D3D9HookSettings::Options::opt_CustomCamera);
               ImGui::Checkbox("Replay Menu", &D3D9HookSettings::Options::opt_ReplayMenu);

               ImGui::End();
            }
            else {
               io.MouseDrawCursor = false;
            }

            if (Mods::NewHUD::confirmSuitableness(io.DeltaTime)) {
               ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
               ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

               static ImVec2 textSize;
               static ImVec2 cursorPos;
               static const float rpmPercentage           = (*Mods::NewHUD::rpm - 1000) / 9000.0f;
               static const float rpmToTextColorIntensity = rpmPercentage * 1.0f;

               // RPM
               {
                  float nos = *Mods::NewHUD::nos;

                  static char rpm[5];
                  sprintf_s(rpm, "%.0f", Mods::NewHUD::getRPM());

                  const ImVec4 bgColor = ImVec4(
                     (0.80f - (0.10f * rpmPercentage)) * nos,
                     (0.56f - (0.15f * rpmPercentage)) * nos,
                     (0.25f - (0.20f * rpmPercentage)) * nos,
                     1.0f);
                  ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);

                  ImGui::SetNextWindowPos(ImVec2(resWidth - 160.0f, resHeight - 70.0f), ImGuiCond_Once);
                  ImGui::Begin("##RPM", (bool*)0, ImVec2(150.0f, 60.0f), 0.7f,
                               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs
                               | ImGuiWindowFlags_ShowBorders);

                  ImGui::PushFont(io.Fonts->Fonts[1]);
                  textSize  = ImGui::CalcTextSize("8888");
                  cursorPos = (ImGui::GetWindowSize() - textSize) / 2 >> 8;

                  ImGui::SetCursorPos(cursorPos);
                  ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 0.3f), "%04.0f", 8888.0f);

                  ImGui::SameLine();

                  ImGui::SetCursorPos(cursorPos);
                  ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), rpm);

                  ImGui::PopFont();
                  ImGui::End();
                  ImGui::PopStyleColor();
               }

               // Speed
               {
                  static char speed[4];

                  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

                  ImGui::SetNextWindowPos(ImVec2(resWidth - 160.0f, resHeight - 150.0f), ImGuiCond_Once);
                  ImGui::Begin("##Speed", (bool*)0, ImVec2(150.0f, 90.0f), 0.0f,
                               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);

                  ImGui::PushFont(io.Fonts->Fonts[2]);
                  sprintf_s(speed, "%03.0f", fabsf(*Mods::NewHUD::speed * 3.6f));
                  textSize = ImGui::CalcTextSize(speed);
                  cursorPos = ImVec2(
                     ImGui::GetWindowWidth() - textSize.x - 10.0f,
                     (ImGui::GetWindowHeight() - textSize.y) / 2.0f + 10.0f
                  );

                  ImGui::SetCursorPos(cursorPos);
                  ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 0.5f), speed);

                  ImGui::SameLine();
                  sprintf_s(speed, "%.0f", fabsf(*Mods::NewHUD::speed * 3.6f));
                  textSize = ImGui::CalcTextSize(speed);
                  cursorPos = ImVec2(
                     ImGui::GetWindowWidth() - textSize.x + 1.0f - 10.0f,
                     (ImGui::GetWindowHeight() - textSize.y) / 2.0f - 2.0f + 10.0f
                  );

                  ImGui::SetCursorPos(cursorPos);
                  ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), speed);

                  ImGui::PopFont();
                  ImGui::End();
                  ImGui::PopStyleVar();
               }

               // Gear
               {
                  static char gearText[5];
                  if (*Mods::NewHUD::gear == 0) {
                     strcpy_s(gearText, "R");
                  }
                  else if (*Mods::NewHUD::gear == 1) {
                     strcpy_s(gearText, "N");
                  }
                  else {
                     _itoa_s(*Mods::NewHUD::gear - 1, gearText, 10);
                  }

                  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

                  ImGui::SetNextWindowPos(ImVec2(resWidth - 175.0f, resHeight - 55.0f), ImGuiCond_Once);
                  ImGui::Begin("##Gear", (bool*)0, ImVec2(30.0f, 30.0f), 0.0f,
                               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs
                               | ImGuiWindowFlags_ShowBorders);

                  static float secondsSinceLastFlash = 0.0f;

                  const ImVec4 frameBg = ImVec4(0.80f - (0.10f * rpmPercentage),
                                                0.56f - (0.15f * rpmPercentage),
                                                0.25f - (0.20f * rpmPercentage),
                                                1.0f);
                  ImVec4 bgColor = ImVec4(1.0f, 0.76f, 0.45f, 1.0f);
                  float textColor = 0.0f;

                  if (Mods::NewHUD::isOverRevving()) {
                     secondsSinceLastFlash += io.DeltaTime;
                     if (secondsSinceLastFlash > 0.2f) {
                        secondsSinceLastFlash = 0.0f;
                     }
                     else if (secondsSinceLastFlash < 0.1f) {
                        bgColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
                        textColor = 1.0f;
                     }
                  }

                  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, bgColor);
                  ImGui::PushStyleColor(ImGuiCol_FrameBg, frameBg);

                  ImGui::ProgressBar(rpmPercentage, ImVec2(30.0f, 30.0f), "");

                  ImGui::PopStyleColor();
                  ImGui::PopStyleColor();

                  ImGui::SetCursorPos((ImGui::GetWindowSize() - ImGui::CalcTextSize(gearText)) / 2);
                  ImGui::TextColored(ImVec4(textColor, textColor, textColor, 1.0f), gearText);

                  ImGui::End();
                  ImGui::PopStyleVar();
               }

               ImGui::PopStyleVar();
               ImGui::PopStyleVar();
            }
            ImGui::Render();
         }
      }

      return origEndScene(pDevice);
   }
   HRESULT WINAPI hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
      if (!D3D9HookSettings::isImguiInitialized)
         return origReset(pDevice, pPresentationParameters);

      ImGui_ImplDX9_InvalidateDeviceObjects();
      auto retOrigReset = origReset(pDevice, pPresentationParameters);
      pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

      if (!pDevice || pDevice->TestCooperativeLevel() != D3D_OK) {
         ImGui_ImplDX9_Shutdown();
         D3D9HookSettings::isImguiInitialized = false;

         return retOrigReset;
      }

      ImGui_ImplDX9_CreateDeviceObjects();
      return retOrigReset;
   }
   HRESULT WINAPI hkBeginStateBlock(LPDIRECT3DDEVICE9 pDevice) {
      d3dDeviceHook->UnhookAll();

      auto retBeginStateBlock = origBeginStateBlock(pDevice);

      origReset           = d3dDeviceHook->Hook(16, hkReset);
      origBeginScene      = d3dDeviceHook->Hook(41, hkBeginScene);
      origEndScene        = d3dDeviceHook->Hook(42, hkEndScene);
      origBeginStateBlock = d3dDeviceHook->Hook(60, hkBeginStateBlock);
      return retBeginStateBlock;
   }

   LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
      if (uMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_KEYMENU)
         return TRUE;

      if (D3D9HookSettings::isImguiInitialized) {
         ImGuiIO io = ImGui::GetIO();

         if (!(io.WantTextInput | io.WantCaptureKeyboard)) {
            if (uMsg == WM_KEYUP) {
               if (wParam == VK_INSERT && !(io.WantTextInput | io.WantCaptureKeyboard))
                  D3D9HookSettings::Options::isMainWindowVisible = !D3D9HookSettings::Options::isMainWindowVisible;
            }
            else if (uMsg == WM_KEYDOWN) {
               switch (wParam) {
                  case VK_END:
                  {
                     static bool mybool = false;
                     mybool             = !mybool;
                     d3dDevice->SetRenderState(D3DRS_FILLMODE, mybool ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
                  }
                  break;
                  default:
                     break;
               }
            }
         }

         if (D3D9HookSettings::Options::isMainWindowVisible) {
            ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam);

            if (io.WantCaptureMouse) {
               switch (uMsg) {
                  case WM_MOUSEACTIVATE:
                  case WM_MOUSEHOVER:
                  case WM_MOUSEHWHEEL:
                  case WM_MOUSELEAVE:
                  case WM_MOUSEMOVE:
                  case WM_MOUSEWHEEL:
                  case WM_LBUTTONDOWN:
                  case WM_LBUTTONUP:
                  case WM_MBUTTONDOWN:
                  case WM_MBUTTONUP:
                  case WM_RBUTTONDOWN:
                  case WM_RBUTTONUP:
                  case WM_XBUTTONDOWN:
                  case WM_XBUTTONUP:
                     D3D9HookSettings::blockMouse = true;
                     return TRUE;
               }
            }
            else {
               D3D9HookSettings::blockMouse = false;
            }

            if (io.WantCaptureKeyboard || io.WantTextInput) {
               D3D9HookSettings::blockKeyboard = true;
               return TRUE;
            }
            else {
               D3D9HookSettings::blockKeyboard = false;

            }
         }
      }

      return CallWindowProc(origWndProc, hWnd, uMsg, wParam, lParam);
   }

   DWORD WINAPI Init(LPVOID) {
      Memory::writeCall(0x2C27D0, (DWORD)gameResolutionCave, false);

      DWORD d3dDeviceAddress = NULL;
      while (!d3dDeviceAddress) {
         d3dDeviceAddress = *(DWORD*)Memory::makeAbsolute(0x582BDC);
         Sleep(100);
      }
      d3dDevice = (LPDIRECT3DDEVICE9)d3dDeviceAddress;
      d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

      D3DDEVICE_CREATION_PARAMETERS cParams;
      d3dDevice->GetCreationParameters(&cParams);
      windowHandle = cParams.hFocusWindow;

      origWndProc = (WNDPROC)SetWindowLongPtr(windowHandle, GWL_WNDPROC, (LONG_PTR)&hkWndProc);

      // fix double clicks
      DWORD Style = GetClassLongPtr(windowHandle, GCL_STYLE) & ~CS_DBLCLKS;
      SetClassLongPtr(windowHandle, GCL_STYLE, Style);

      d3dDeviceHook       = make_unique<VTableHook>((PDWORD*)d3dDevice);
      origReset           = d3dDeviceHook->Hook(16, hkReset);
      origBeginScene      = d3dDeviceHook->Hook(41, hkBeginScene);
      origEndScene        = d3dDeviceHook->Hook(42, hkEndScene);
      origBeginStateBlock = d3dDeviceHook->Hook(60, hkBeginStateBlock);

      showUserGuide = Settings::isFirstTime();

      return TRUE;
   }
}