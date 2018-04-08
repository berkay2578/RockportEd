#include "stdafx.h"
#include "InGameMenu.h"
#include "Extensions\Extensions.h"
// Helpers
#include "Helpers\Settings\Settings.h"
#include "Helpers\WndProc\WndProcHook.h"
// dear imgui
#include "Helpers\imgui\extra_fonts\RobotoMedium.hpp"
#include "Helpers\imgui\extra_fonts\CooperHewitt_Roman.hpp"
#include "Helpers\imgui\extra_fonts\CooperHewitt_Bold.hpp"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "Helpers\imgui\dx9\imgui_impl_dx9.h"

namespace Extensions {
   namespace InGameMenu {
      _BaseInGameMenuItem*              activeItem = nullptr;
      std::vector<_BaseInGameMenuItem*> items      = {};

      bool isImguiInitialized  = false;
      bool isMainWindowVisible = true;
      bool isFirstTimeUser     = false;

      void showUserGuide() {
         if (isFirstTimeUser) {
            if (ImGui::Begin("User Guide", &isFirstTimeUser)) {
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
               ImGui::BulletText("CTRL+MouseWheel to scale the UI.");
               ImGui::BulletText("Click on the button at the top-right of this window to close it.");
            }
            ImGui::End();

            if (Settings::settingsType.isFirstTimeUser) {
               Settings::settingsType.isFirstTimeUser = false;
               Settings::saveSettings();
            }
         }
      }

      void WINAPI beginScene(LPDIRECT3DDEVICE9 pDevice) {
         if (!isImguiInitialized) {
            ImGui_ImplDX9_Init(Helpers::WndProcHook::windowHandle, pDevice);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(CooperHewitt_Roman_compressed_data, CooperHewitt_Roman_compressed_size, 36.0f);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(CooperHewitt_Bold_compressed_data, CooperHewitt_Bold_compressed_size, 36.0f);
            imguiIO->FontDefault = NULL;
            imguiIO->IniFilename = NULL;

            imguiIO->FontAllowUserScaling = true;
            ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions
               | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_PickerHueWheel);
            ImGui::LoadStyle();
            isImguiInitialized = true;
         }

         ImGui_ImplDX9_NewFrame();
      }
      void WINAPI endScene(LPDIRECT3DDEVICE9 pDevice) {
         if (isImguiInitialized) {
            if (isMainWindowVisible) {
               imguiIO->MouseDrawCursor = imguiIO->WantCaptureMouse;
               showUserGuide();

               ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f), ImGuiCond_Once);
               if (ImGui::Begin("##RockportEd_Main", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
                  ImGui::PushFont(imguiIO->Fonts->Fonts[1]);
                  ImGui::Text("Rockport");
                  ImVec2 diff = ImGui::CalcTextSize("Rockport");
                  ImGui::PopFont();
                  ImGui::PushFont(imguiIO->Fonts->Fonts[2]);
                  ImGui::SameLine(10.0f + diff.x); ImGui::SetCursorPosY(1.0f + ImGui::GetStyle().WindowPadding.y);
                  ImGui::TextColored(ImVec4(1.0f, 0.565f, 0.0f, 1.0f), "Ed");
                  ImGui::PopFont();
                  ImGui::SameLine(0.0f, 3.0f);
                  const float buttonWidth = ImGui::GetCursorPos().x + ImGui::CalcTextSize("v1.0").x;
                  ImGui::Text("v1.0");

                  if (activeItem) {
                     if (ImGui::Button("< Back"))
                        activeItem = nullptr;
                     else {
                        activeItem->onFrame();
                        if (activeItem->displayMenu())
                           activeItem = nullptr;
                     }
                  }
                  for (auto item : items) {
                     if (item->hasLoadedData) {
                        if (activeItem != item)
                           item->onFrame();
                        if (!activeItem && item->displayMenuItem(ImVec2(buttonWidth, 0.0f)))
                           activeItem = item;
                     }
                  }
               }
               ImGui::End();
            } else {
               imguiIO->MouseDrawCursor = false;
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
         } else {
            pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
         }

         ImGui_ImplDX9_CreateDeviceObjects();
      }

      LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
         if (isImguiInitialized) {
            if (uMsg == WM_QUIT) {
               ImGui_ImplDX9_Shutdown();
               return FALSE;
            }

            if (isMainWindowVisible)
               ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

            if (!(imguiIO->WantTextInput | imguiIO->WantCaptureKeyboard)) {
               if (uMsg == WM_KEYUP) {
                  switch (wParam) {
                     case VK_INSERT:
                        isMainWindowVisible = !isMainWindowVisible;
                        return TRUE;
                  }
               }
            }
         }
         return FALSE;
      }

      void Init() {
         isFirstTimeUser = Settings::settingsType.isFirstTimeUser;

         LPDIRECT3DDEVICE9 d3dDevice = nullptr;
         while (!d3dDevice) {
            d3dDevice = MirrorHook::D3D9::GetD3D9Device();
            Sleep(100);
         }
         d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::BeginScene, &beginScene);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::EndScene, &endScene);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::BeforeReset, &beforeReset);
         MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::AfterReset, &afterReset);

         Helpers::WndProcHook::addExtension(&wndProcExtension);
      }
   }
}