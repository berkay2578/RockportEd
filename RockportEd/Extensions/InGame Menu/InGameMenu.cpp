/*
   MIT License
   
   Copyright (c) 2017-2018 Berkay Yiðit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#include "stdafx.h"
#include "InGameMenu.h"
#include "Extensions\Extensions.h"
// Helpers
#include "Helpers\WndProc\WndProcHook.h"
// dear imgui
#include "Helpers\imgui\extra_fonts\RobotoMedium.hpp"
#include "Helpers\imgui\extra_fonts\CooperHewitt_Roman.hpp"
#include "Helpers\imgui\extra_fonts\CooperHewitt_Bold.hpp"
#include "Helpers\imgui\extra_fonts\Aramis_Book_Italic.hpp"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "Helpers\imgui\dx9\imgui_impl_dx9.h"

namespace Extensions {
   ImGuiIO* imguiIO = nullptr;
   namespace InGameMenu {
      _BaseInGameMenuItem*              activeItem = nullptr;
      std::vector<_BaseInGameMenuItem*> items      = {};

      bool isImguiInitialized  = false;
      bool isMainWindowVisible = true;
      bool isFirstTimeUser     = false;

      RECT clientRect = { 0 };

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
            Extensions::imguiIO = &ImGui::CreateContext()->IO;
            ImGui_ImplDX9_Init(Helpers::WndProcHook::windowHandle, pDevice);

            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 21.0f);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(CooperHewitt_Roman_compressed_data, CooperHewitt_Roman_compressed_size, 64.0f);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(CooperHewitt_Bold_compressed_data, CooperHewitt_Bold_compressed_size, 64.0f);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(Aramis_Book_Italic::Aramis_Book_Italic_compressed_data, Aramis_Book_Italic::Aramis_Book_Italic_compressed_size, 22.0f);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(Aramis_Book_Italic::Aramis_Book_Italic_compressed_data, Aramis_Book_Italic::Aramis_Book_Italic_compressed_size, 100.0f);
            imguiIO->Fonts->Build(); // why is this here again?
            imguiIO->FontDefault = imguiIO->Fonts->Fonts[0];
            imguiIO->IniFilename = NULL;

            ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions
                                       | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_PickerHueWheel);
            ImGui::LoadStyle();
            isImguiInitialized = true;
         }

         ImGui_ImplDX9_NewFrame();

         static float scaling = 1.0f;
         GetClientRect(Helpers::WndProcHook::windowHandle, &clientRect);
         scaling = max(0.6f, (float)clientRect.right / 1920.0f); // optimized for 1080p
         for (int i = 0; i < 3; i++) { // don't change custom hud fonts
            imguiIO->Fonts->Fonts[i]->Scale = scaling;
         }

         for (auto item : items) {
            if (item->hasLoadedData) {
               item->onFrame();
            }
         }
      }
      void WINAPI endScene(LPDIRECT3DDEVICE9 pDevice) {
         if (isImguiInitialized) {
            if (isMainWindowVisible) {
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

                  if (!activeItem) {
                     for (auto item : items) {
                        if (item->hasLoadedData && item->displayMenuItem(ImVec2(buttonWidth, 0.0f)))
                           activeItem = item;
                     }
                  } else {
                     if (ImGui::Button("< Back"))
                        activeItem = nullptr;
                     else {
                        if (!activeItem->displayMenu())
                           activeItem = nullptr;
                     }
                  }
               }
               ImGui::End();

               imguiIO->MouseDrawCursor = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
            } else {
               imguiIO->MouseDrawCursor = false;
            }
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
         }
      }

      void WINAPI beforeReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
         if (isImguiInitialized)
            ImGui_ImplDX9_InvalidateDeviceObjects();
      }
      void WINAPI afterReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
         if (!pDevice || pDevice->TestCooperativeLevel() != D3D_OK) {
            if (isImguiInitialized) {
               ImGui_ImplDX9_Shutdown();
               ImGui::DestroyContext();
            }
            isImguiInitialized = false;
            return;
         } else {
            pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
         }

         ImGui_ImplDX9_CreateDeviceObjects();
      }

      LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
         if (isImguiInitialized) {
            if (uMsg == WM_QUIT) {
               ImGui_ImplDX9_Shutdown();
               ImGui::DestroyContext();
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
         while (!isImguiInitialized) Sleep(100);
      }
   }
}