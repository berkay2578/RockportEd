#include "stdafx.h"
#include "D3D9Hook.h"
#include "DInput8Hook.h"
#include "Memory.h"
#include "Mods.h"
using std::map;

#include <d3d9.h>
#include MIRRORHOOK_DEFINITIONS_PATH

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

#include "imgui\imgui.h"
#include "imgui_ext.h"
#include "imgui\extra_fonts\RobotoMedium.hpp"
#include "imgui\extra_fonts\Digital.hpp"
#include "imgui\extra_fonts\CooperHewitt_Italic.hpp"

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "imgui\dx9\imgui_impl_dx9.h"

//SetWindowSubClass
#include <commctrl.h>

namespace D3D9Hook {
   namespace Settings {
      // Exposed variables
      bool blockKeyboard  = false;
      bool blockMouse     = false;

      // Private variables
      map<HookOptions, bool> enabledOptions =
      {
         { HookOptions::CustomCamera, false },
         { HookOptions::Replay,       false },
         { HookOptions::NewHUD,       true },
         { HookOptions::GameSpeed,    false }
      };
      map<HookOptions, bool> visibleMenus =
      {
         { HookOptions::CustomCamera, false },
         { HookOptions::Replay,       false },
         { HookOptions::NewHUD,       false },
         { HookOptions::GameSpeed,    false }
      };
   }

   // Hook variables
   LPDIRECT3DDEVICE9 d3dDevice    = nullptr;
   HWND              windowHandle = nullptr;
   WNDPROC           origWndProc  = nullptr;

   // Namespace variables
   bool isImguiInitialized  = false;
   bool isMainWindowVisible = true;
   bool showUserGuide       = false;
   char* cameras[7]         =
   {
      "Bumper",
      "Hood",
      "Near",
      "Far",
      "Challenge Entry",
      "Speedbreaker",
      "Pullback"
   };

   // Game variables
   long* game_MousePosX;
   long* game_MousePosY;
   float game_ResWidth;
   float game_ResHeight;
   float baseResWidth;
   float baseResHeight;

   void WINAPI beginScene(LPDIRECT3DDEVICE9 pDevice) {
      if (!isImguiInitialized) {
         ImGui_ImplDX9_Init(windowHandle, d3dDevice);

         ImGuiIO& io = ImGui::GetIO();
         io.IniFilename = NULL;
         io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
         io.Fonts->AddFontFromMemoryCompressedTTF(Digital::Digital_compressed_data, Digital::Digital_compressed_size, 34.0f);
         io.Fonts->AddFontFromMemoryCompressedTTF(CooperHewitt_Italic_compressed_data, CooperHewitt_Italic_compressed_size, 64.0f);
         io.FontDefault = NULL;

         ImGui::LoadStyle();
         isImguiInitialized = true;
      }

      ImGui_ImplDX9_NewFrame();
   }
   void WINAPI endScene(LPDIRECT3DDEVICE9 pDevice) {
      if (isImguiInitialized) {
         ImGuiIO& io = ImGui::GetIO();

         if (isMainWindowVisible) {
            io.MousePos.x      = (float)(*game_MousePosX) * ((float)game_ResWidth / baseResWidth);
            io.MousePos.y      = (float)(*game_MousePosY) * ((float)game_ResHeight / baseResHeight);
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
            if (Settings::visibleMenus[HookOptions::CustomCamera]) {
               ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
               ImGui::Begin("Custom Camera", &Settings::visibleMenus[HookOptions::CustomCamera], ImVec2(200.0f, 200.0f), 0.8f);
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
            if (Settings::visibleMenus[HookOptions::Replay]) {
               ImGui::SetNextWindowPosCenter(ImGuiCond_Once);
               ImGui::Begin("Replay System", &Settings::visibleMenus[HookOptions::Replay], ImVec2(160.0f, 180.0f), 0.8f);

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

            ImGui::Checkbox("Camera Settings", &Settings::visibleMenus[HookOptions::CustomCamera]);
            ImGui::Checkbox("Replay Menu", &Settings::visibleMenus[HookOptions::Replay]);
            ImGui::Checkbox("New HUD", &Settings::enabledOptions[HookOptions::NewHUD]);
            ImGui::TextWrapped("Gameplay Speed");
            ImGui::SliderFloat("##GameplaySpeed", Mods::GameInfo::gameplaySpeed, 0.0f, 1.5f);

            ImGui::End();
         }
         else {
            io.MouseDrawCursor = false;
         }

         if (Settings::enabledOptions[HookOptions::NewHUD] && Mods::NewHUD::confirmSuitableness(io.DeltaTime)) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

            static ImVec2 textSize;
            static ImVec2 cursorPos;
            const float rpmPercentage           = (*Mods::NewHUD::rpm - 1000.0f) / 9000.0f;
            const float rpmToTextColorIntensity = rpmPercentage * 1.0f;
            const float throttlePercentage      = *Mods::NewHUD::throttlePercentage / 100.0f;
            const float nos                     = *Mods::NewHUD::nos;

            // RPM
            {
               static char rpm[5];
               sprintf_s(rpm, "%.0f", Mods::NewHUD::getRPM());

               const ImVec4 bgColor = ImVec4(
                  0.80f - (0.10f * rpmPercentage),
                  0.56f - (0.15f * rpmPercentage),
                  0.25f - (0.20f * rpmPercentage),
                  0.7f);
               ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
               ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

               ImGui::SetNextWindowPos(ImVec2(game_ResWidth - 160.0f, game_ResHeight - 70.0f), ImGuiCond_Once);
               ImGui::Begin("##RPM", (bool*)0, ImVec2(150.0f, 60.0f), 0.0f,
                            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs
                            | ImGuiWindowFlags_ShowBorders);

               ImGui::PushStyleColor(ImGuiCol_PlotHistogram, bgColor);
               ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));

               ImGui::ProgressBar(nos, ImVec2(150.0f, 60.0f), "");

               ImGui::PopStyleColor();
               ImGui::PopStyleColor();

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
               ImGui::PopStyleVar();
            }

            // Speed
            {
               static char speed[4];

               ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

               ImGui::SetNextWindowPos(ImVec2(game_ResWidth - 160.0f, game_ResHeight - 150.0f), ImGuiCond_Once);
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
               ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), speed);

               ImGui::PopFont();
               ImGui::End();
               ImGui::PopStyleVar();
            }

            // Gear
            {
               static char gearText[5];
               const int gear = *Mods::NewHUD::gear;
               switch (gear) {
                  case 0:
                     strcpy_s(gearText, "R");
                     break;
                  case 1:
                     strcpy_s(gearText, "N");
                     break;
                  default:
                     _itoa_s(*Mods::NewHUD::gear - 1, gearText, 10);
                     break;
               }
               ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

               ImGui::SetNextWindowPos(ImVec2(game_ResWidth - 175.0f, game_ResHeight - 55.0f), ImGuiCond_Once);
               ImGui::Begin("##Gear", (bool*)0, ImVec2(30.0f, 30.0f), 0.0f,
                            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs
                            | ImGuiWindowFlags_ShowBorders);

               static float secondsSinceLastFlash = 0.0f;

               const ImVec4 frameBg = ImVec4(0.80f - (0.10f * rpmPercentage),
                                             0.56f - (0.15f * rpmPercentage),
                                             0.25f - (0.20f * rpmPercentage),
                                             1.0f);
               ImVec4 bgColor = gear == 0 ? ImVec4(0.0f, 0.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.76f, 0.45f, 1.0f);
               float textColor = gear == 0 ? 1.0f : 0.0f;

               if (Mods::NewHUD::isOverRevving()) {
                  secondsSinceLastFlash += io.DeltaTime;
                  if (secondsSinceLastFlash > (0.2f / throttlePercentage)) {
                     secondsSinceLastFlash = 0.0f;
                  }
                  else if (secondsSinceLastFlash < (0.1f / throttlePercentage)) {
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
   void WINAPI beforeReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
      if (isImguiInitialized)
         ImGui_ImplDX9_InvalidateDeviceObjects();
   }
   void WINAPI afterReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
      pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

      if (!pDevice || pDevice->TestCooperativeLevel() != D3D_OK) {
         ImGui_ImplDX9_Shutdown();
         isImguiInitialized = false;
      }

      ImGui_ImplDX9_CreateDeviceObjects();
   }

   LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
      // Disable ALT menu
      if (uMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_KEYMENU)
         return TRUE;
      // Disable game's 'stop on focus loss'
      if (uMsg == WM_ACTIVATEAPP) {
         if (wParam == FALSE)
            return TRUE;
      }
      // Disable Windows mouse
      if (uMsg == WM_SETCURSOR && LOWORD(lParam) == HTCLIENT) {
         SetCursor(NULL);
         return TRUE;
      }

      if (isImguiInitialized) {
         ImGuiIO io = ImGui::GetIO();

         if (!(io.WantTextInput | io.WantCaptureKeyboard)) {
            if (uMsg == WM_KEYUP) {
               if (wParam == VK_INSERT && !(io.WantTextInput | io.WantCaptureKeyboard))
                  isMainWindowVisible = !isMainWindowVisible;
            }
            else if (uMsg == WM_KEYDOWN) {
               switch (wParam) {
                  case VK_PAUSE:
                  {
                     *Mods::GameInfo::gameplaySpeed = 0.0f;
                     break;
                  }
               }

               if (Mods::NewHUD::gear
                   && Mods::GameInfo::isManualTransmissionEnabled
                   && *Mods::GameInfo::isManualTransmissionEnabled
                   ) {
                  if (wParam == MapVirtualKeyEx(*Mods::GameInfo::key_GearDown, MAPVK_VSC_TO_VK, GetKeyboardLayout(NULL))) {
                     Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear = max(0, *Mods::NewHUD::gear - 1);
                     if (Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear == 0) {
                        DInput8Hook::reversePedals  = true;
                     }
                  }
                  else if (wParam == MapVirtualKeyEx(*Mods::GameInfo::key_GearUp, MAPVK_VSC_TO_VK, GetKeyboardLayout(NULL))) {
                     Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear = max(1, *Mods::NewHUD::gear + 1);
                     if (Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear > 0) {
                        DInput8Hook::reversePedals  = false;
                     }
                  }
               }
            }
         }

         if (isMainWindowVisible) {
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
                     Settings::blockMouse = true;
                     return TRUE;
               }
            }
            else {
               Settings::blockMouse = false;
            }

            if (/*io.WantCaptureKeyboard ||*/ io.WantTextInput) {
               Settings::blockKeyboard = true;
               return TRUE;
            }
            else {
               Settings::blockKeyboard = false;
            }
         }
      }

      return CallWindowProc(origWndProc, hWnd, uMsg, wParam, lParam);
   }

   int gameResolutionCave() {
      int currentResIndex = *(int*)Memory::makeAbsolute(0x50181C);
      if (currentResIndex < 5) {
         DWORD* newResolutionSetupAddrs;
         newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2870);
         game_ResWidth                = (float)*(int*)(newResolutionSetupAddrs[currentResIndex] + 0x0A);
         game_ResHeight               = (float)*(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10);

         int ratio = (int)((game_ResWidth / game_ResHeight) * 100);
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
   DWORD WINAPI Init(LPVOID) {
      Memory::writeCall(0x2C27D0, (DWORD)gameResolutionCave, false);

      bool argWindowed = *(bool*)0x982BF0;
      game_MousePosX = (long*)Memory::makeAbsolute(0x51CFB0);
      game_MousePosY = (long*)Memory::makeAbsolute(0x51CFB4);

      HMODULE hMirrorHook = nullptr;
      while (!hMirrorHook) {
         hMirrorHook = GetModuleHandle("MirrorHook.asi");
         Sleep(100);
      }

      while (!MirrorHook::IsReady()) {
         *(bool*)0x982BF0 = true; // force windowed mode
         Sleep(100);
      }

      while (!d3dDevice) {
         d3dDevice = MirrorHook::D3D9::GetD3D9Device();
         Sleep(100);
      }

      d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
      MirrorHook::D3D9::AddD3D9Extender(MirrorHook::D3D9::D3D9Extender::BeforeReset, &beforeReset);
      MirrorHook::D3D9::AddD3D9Extender(MirrorHook::D3D9::D3D9Extender::AfterReset, &afterReset);
      MirrorHook::D3D9::AddD3D9Extender(MirrorHook::D3D9::D3D9Extender::BeginScene, &beginScene);
      MirrorHook::D3D9::AddD3D9Extender(MirrorHook::D3D9::D3D9Extender::EndScene, &endScene);

      windowHandle = MirrorHook::D3D9::GetWindowHandle();
      origWndProc = (WNDPROC)SetWindowLongPtr(windowHandle, GWL_WNDPROC, (LONG_PTR)&hkWndProc);

      showUserGuide = false; //Settings::isFirstTime();

      if (argWindowed) { // Proper windowed
         RECT o_cRect, n_cRect, n_wRect;
         GetClientRect(windowHandle, &o_cRect);

         DWORD wStyle = GetWindowLongPtr(windowHandle, GWL_STYLE) | WS_OVERLAPPEDWINDOW & (~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
         SetWindowLongPtr(windowHandle, GWL_STYLE, wStyle);

         // make window change style
         SetWindowPos(windowHandle, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);

         GetWindowRect(windowHandle, &n_wRect);
         GetClientRect(windowHandle, &n_cRect);
         int n_wWidth  = n_wRect.right - n_wRect.left;
         int n_wHeight = n_wRect.bottom - n_wRect.top;
         int dif_wWidth  = o_cRect.right - n_cRect.right;
         int dif_wHeight = o_cRect.bottom - n_cRect.bottom;
         int newWidth  = n_wWidth + dif_wWidth;
         int newHeight = n_wHeight + dif_wHeight;

         SetWindowPos(windowHandle, NULL, 0, 0, newWidth, newHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_DRAWFRAME);
      }
      else { // Borderless fullscreen
         SetWindowPos(windowHandle, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE);
      }
      *(bool*)0x982BF0 = argWindowed; // restore

      return TRUE;
   }
}