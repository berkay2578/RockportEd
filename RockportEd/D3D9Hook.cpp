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

#include "imgui/imgui.h"
#include "imgui/extra_fonts/RobotoMedium.hpp"

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "imgui/dx9/imgui_impl_dx9.h"

#include <memory>
using std::unique_ptr;
using std::make_unique;

//SetWindowSubClass
#include <commctrl.h>

namespace D3D9Hook {
   HWND windowHandle = nullptr;
   WNDPROC origWndProc;
   DWORD d3dDeviceAddress;
   LPDIRECT3DDEVICE9 d3dDevice = nullptr;
   unique_ptr<VTableHook> d3dDeviceHook = nullptr;

   Reset_t origReset = nullptr;
   BeginScene_t origBeginScene = nullptr;
   EndScene_t origEndScene = nullptr;
   BeginStateBlock_t origBeginStateBlock = nullptr;

   bool showUserGuide = false;
   char* cameras[7] ={
      "Bumper",
      "Hood",
      "Near",
      "Far",
      "i",
      "don kno",
      "Pullback"
   };

   void doImGuiStyle() {
      ImGuiStyle* style = &ImGui::GetStyle();

      style->WindowMinSize = ImVec2(100.0f, 100.0f);
      style->WindowPadding = ImVec2(10.0f, 10.0f);
      style->WindowRounding = 5.0f;
      style->ChildWindowRounding = 5.0f;
      style->FramePadding = ImVec2(5.0f, 4.0f);
      style->FrameRounding = 5.0f;
      style->ItemSpacing = ImVec2(5.0f, 5.0f);
      style->ItemInnerSpacing = ImVec2(10.0f, 10.0f);
      style->IndentSpacing = 15.0f;
      style->ScrollbarSize = 16.0f;
      style->ScrollbarRounding = 5.0f;
      style->GrabMinSize = 7.0f;
      style->GrabRounding = 2.0f;

      style->Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
      style->Colors[ImGuiCol_WindowBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
      style->Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);
      style->Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
      style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_FrameBg] = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
      style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
      style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
      style->Colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.8f);
      style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.78f, 0.78f, 0.78f, 0.39f);
      style->Colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
      style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
      style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
      style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
      style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
      style->Colors[ImGuiCol_ComboBg] = ImVec4(0.78f, 0.78f, 0.78f, 0.98f);
      style->Colors[ImGuiCol_CheckMark] = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
      style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
      style->Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
      style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
      style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
      style->Colors[ImGuiCol_Header] = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
      style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.51f, 0.67f, 1.00f);
      style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
      style->Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
      style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
      style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.27f, 0.59f, 0.75f, 0.78f);
      style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.08f, 0.39f, 0.55f, 0.78f);
      style->Colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
      style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.71f, 0.71f, 0.71f, 0.60f);
      style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
      style->Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
      style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
      style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
      style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
   }

   HRESULT WINAPI beginSceneHook(LPDIRECT3DDEVICE9 pDevice) {
      if (!D3D9HookSettings::isImguiInitialized) {
         ImGui_ImplDX9_Init(windowHandle, d3dDevice);
         ImGuiIO& io = ImGui::GetIO();
         io.IniFilename = NULL;
         io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
         io.FontDefault = NULL;
         D3D9HookSettings::isImguiInitialized = true;
      }

      ImGui_ImplDX9_NewFrame();
      doImGuiStyle();

      return origBeginScene(pDevice);
   }

   float resWidth, resHeight;
   float baseResWidth, baseResHeight;
   int gameResolutionCave() {
      int currentResIndex = *(int*)Memory::makeAbsolute(0x50181C);
      if (currentResIndex < 5) {
         DWORD* newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2870);
         resWidth = (float)*(int*)(newResolutionSetupAddrs[currentResIndex] + 0xA);
         resHeight = (float)*(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10);

         int ratio = (int)((resWidth / resHeight) * 100);
         if (ratio == 177) { // 16:9
            baseResWidth = 850.0f;
            baseResHeight = 480.0f;
         }
         else if (ratio == 133) { // 4:3
            baseResWidth = 640.0f - 3; // -3
            baseResHeight = 480.0f - 5; // -5
         }
      }
      return currentResIndex;
   }

   HRESULT WINAPI endSceneHook(LPDIRECT3DDEVICE9 pDevice) {
      if (pDevice->TestCooperativeLevel() == D3D_OK) {
         if (D3D9HookSettings::isImguiInitialized) {
            ImGuiIO& o = ImGui::GetIO();

            if (D3D9HookSettings::Options::isMainWindowVisible) {
               o.MousePos.x = (float)(*(LONG*)Memory::makeAbsolute(0x51CFB0)) * ((float)resWidth / baseResWidth);
               o.MousePos.y = (float)(*(LONG*)Memory::makeAbsolute(0x51CFB4)) * ((float)resHeight / baseResHeight);
               o.MouseDrawCursor = o.WantCaptureMouse;

               if (showUserGuide) {
                  ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
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
                  ImGui::SetNextWindowPosCenter(ImGuiSetCond_Once);
                  ImGui::Begin("Custom Camera", &D3D9HookSettings::Options::opt_CustomCamera, ImVec2(200.0f, 200.0f), 0.8f);
                  ImGui::PushItemWidth(-1);
                  ImGui::TextWrapped("Camera");
                  ImGui::Combo("##CameraCombo", Mods::activeCamera, cameras, 7);
                  int activeCam = *Mods::activeCamera;
                  if (activeCam == 0 || (activeCam > 1 && activeCam < 4)) {
                     ImGui::TextWrapped("X");
                     ImGui::SliderFloat("##CameraX", Mods::cameraData[activeCam]["X"], -10.0f, 10.0f);
                     ImGui::TextWrapped("Z");
                     ImGui::SliderFloat("##CameraZ", Mods::cameraData[activeCam]["Z"], -10.0f, 10.0f);
                     ImGui::TextWrapped("FOV");
                     ImGui::SliderFloat("##CameraFov", Mods::cameraData[activeCam]["Fov"], 25.0f, 120.0f);
                     ImGui::TextWrapped("Horizontal Angle");
                     ImGui::SliderFloat("##CameraHorAngle", Mods::cameraData[activeCam]["HorAngle"], -45.0f, 45.0f, "%.3f deg");
                     ImGui::TextWrapped("Vertical Angle");
                     ImGui::SliderFloat("##CameraVerAngle", Mods::cameraData[activeCam]["VerAngle"], -25.0f, 45.0f, "%.3f deg");
                  }
                  ImGui::PopItemWidth();
                  ImGui::End();
               }
               if (D3D9HookSettings::Options::opt_ReplayMenu) {
                  ImGui::SetNextWindowPosCenter(ImGuiSetCond_Once);
                  ImGui::Begin("Replay System", &D3D9HookSettings::Options::opt_ReplayMenu, ImVec2(160.0f, 180.0f), 0.8f);
                  if (ImGui::Checkbox("Record", &Mods::isRecording)) {
                     if (Mods::isRecording)
                        Mods::startRecording();
                  }
                  ImGui::Text("Frame Count: %u", Mods::frameCount);

                  ImGui::Separator();

                  if (ImGui::Checkbox("Replay", &Mods::isShowingReplay)) {
                     if (Mods::isShowingReplay)
                        Mods::startReplay();
                  }
                  if (Mods::isShowingReplay) {
                     if (ImGui::Checkbox("Pause", &Mods::isReplayPaused)) {
                        Mods::changeReplayState();
                     }
                     ImGui::SliderUInt("Current Frame", &Mods::frameNr, 0, Mods::frameCount - 1);
                  }
                  ImGui::End();
               }

               ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiSetCond_Once);
               ImGui::Begin("RockportEd", (bool*)0, ImVec2(200.0f, 140.0f), 0.9f);

            #ifdef NDEBUG
               ImGui::TextWrapped("Current car bytes: %s", D3D9HookSettings::Options::opt_CustomCarBytesValue);
            #else
               ImGui::Checkbox("Car bytes: ", &D3D9HookSettings::Options::opt_CustomCarBytes);
               ImGui::InputText("##CarBytes", D3D9HookSettings::Options::opt_CustomCarBytesValue, 256,
                                ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase
                                | (D3D9HookSettings::Options::opt_CustomCarBytes ? 0 : ImGuiInputTextFlags_ReadOnly));
            #endif
               ImGui::Checkbox("Camera Settings", &D3D9HookSettings::Options::opt_CustomCamera);
               ImGui::Checkbox("Replay Menu", &D3D9HookSettings::Options::opt_ReplayMenu);

               ImGui::End();
            }
            else {
               o.MouseDrawCursor = false;
            }
            ImGui::Render();
         }
      }

      return origEndScene(pDevice);
   }
   HRESULT WINAPI resetHook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
      if (!D3D9HookSettings::isImguiInitialized)
         return origReset(pDevice, pPresentationParameters);

      ImGui_ImplDX9_InvalidateDeviceObjects();
      auto retOrigReset = origReset(pDevice, pPresentationParameters);

      if (!pDevice || pDevice->TestCooperativeLevel() != D3D_OK) {
         ImGui_ImplDX9_Shutdown();
         D3D9HookSettings::isImguiInitialized = false;

         return retOrigReset;
      }

      ImGui_ImplDX9_CreateDeviceObjects();
      return retOrigReset;
   }
   HRESULT WINAPI beginStateBlockHook(LPDIRECT3DDEVICE9 pDevice) {
      d3dDeviceHook->UnhookAll();

      auto retBeginStateBlock = origBeginStateBlock(pDevice);

      origReset = d3dDeviceHook->Hook(16, resetHook);
      origBeginScene = d3dDeviceHook->Hook(41, beginSceneHook);
      origEndScene = d3dDeviceHook->Hook(42, endSceneHook);
      origBeginStateBlock = d3dDeviceHook->Hook(60, beginStateBlockHook);
      return retBeginStateBlock;
   }

   LRESULT CALLBACK WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
      if (uMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_KEYMENU)
         return TRUE;

      if (D3D9HookSettings::isImguiInitialized) {
         ImGuiIO io = ImGui::GetIO();

         if (uMsg == WM_KEYUP) {
            if (wParam == VK_INSERT && !(io.WantTextInput | io.WantCaptureKeyboard))
               D3D9HookSettings::Options::isMainWindowVisible = !D3D9HookSettings::Options::isMainWindowVisible;
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

      while (!d3dDeviceAddress) {
         d3dDeviceAddress = *(DWORD*)Memory::makeAbsolute(0x582BDC);
         Sleep(100);
      }
      d3dDevice = (LPDIRECT3DDEVICE9)d3dDeviceAddress;

      D3DDEVICE_CREATION_PARAMETERS cParams;
      d3dDevice->GetCreationParameters(&cParams);
      windowHandle = cParams.hFocusWindow;

      origWndProc = (WNDPROC)SetWindowLongPtr(windowHandle, GWL_WNDPROC, (LONG_PTR)&WndProcHook);

      // fix double clicks
      DWORD Style = GetClassLongPtr(windowHandle, GCL_STYLE) & ~CS_DBLCLKS;
      SetClassLongPtr(windowHandle, GCL_STYLE, Style);

      d3dDeviceHook = make_unique<VTableHook>((PDWORD*)d3dDevice);
      origReset = d3dDeviceHook->Hook(16, resetHook);
      origBeginScene = d3dDeviceHook->Hook(41, beginSceneHook);
      origEndScene = d3dDeviceHook->Hook(42, endSceneHook);
      origBeginStateBlock = d3dDeviceHook->Hook(60, beginStateBlockHook);

      showUserGuide = Settings::isFirstTime();

      return TRUE;
   }
}