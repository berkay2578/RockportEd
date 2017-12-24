#include "stdafx.h"
#include "InGameMenu.h"
#include "Extensions\Extensions.h"
// Helpers
#include "Helpers\Settings\Settings.h"
#include "Helpers\WndProc\WndProcHook.h"
// dear imgui
#include "Helpers\imgui\imgui_ext.h"
#include "Helpers\imgui\extra_fonts\RobotoMedium.hpp"
//#include "Helpers\imgui\extra_fonts\Aramis_Book_Italic.hpp"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include "Helpers\imgui\dx9\imgui_impl_dx9.h"

namespace Extensions {
   namespace InGameMenu {
      std::vector<_BaseInGameMenuItem*> items ={};

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
            ImGui_ImplDX9_Init(Hooks::WndProc::windowHandle, pDevice);
            imguiIO->Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
            imguiIO->FontDefault = NULL;
            imguiIO->IniFilename = NULL;

            ImGui::SetColorEditOptions(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions
                                       | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_PickerHueWheel);
            ImGui::LoadStyle();
            isImguiInitialized = true;
         }

         ImGui_ImplDX9_NewFrame();
      }
      void WINAPI endScene(LPDIRECT3DDEVICE9 pDevice) {
         if (isImguiInitialized) {
            ImGui::SetNextWindowPos(ImVec2(imguiIO->DisplaySize.x - 5.0f, 5.0f), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            ImGui::Begin("##Huh", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "RockportEd debug build");
            ImGui::Bullet(); ImGui::TextWrapped("Menu input <-> game input RETURN(Enter) KeyUp is broken.");
            ImGui::Bullet(); ImGui::TextWrapped("Lighting Editor only works on some save games and is unstable. TODO: Update pointer to a vtable*.");
            ImGui::End();

            if (isMainWindowVisible) {
               imguiIO->MouseDrawCursor = imguiIO->WantCaptureMouse;
               showUserGuide();

               ImGui::SetNextWindowPos(ImVec2(10.0f, 5.0f), ImGuiCond_Once);
               if (ImGui::Begin("RockportEd", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  for (auto item : items) {
                     if (item->hasLoadedData) {
                        item->displayMenu();
                     }
                  }
               }
               ImGui::End();

               /* DEBUGGGGGGGGGG: That's the way, aha, aha. I like it, aha, aha.
               struct PhyicsTuning {
                  float steering = 0.0f;
                  float handling = 0.0f;
                  float brakes = 0.0f;
                  float rideheight = 0.0f;
                  float aero = 0.0f;
                  float nos = 0.0f;
                  float turbo = 0.0f;
               };

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

         Hooks::WndProc::addExtension(&wndProcExtension);
      }
   }
}