#pragma once
#include "stdafx.h"
#include "Memory.h"
#include "WndProcHook.h"

#include <d3d9.h>
#include MIRRORHOOK_DEFINITIONS_PATH

namespace Extensions {
   namespace WndProc {
      namespace WindowedModeImprovements {
         // TODO: Expose on SettingsType
         static bool properWindowStyle   = true;
         static bool allowWindowResizing = true;

         static RECT clientRect = { 0 };
         static bool forceReset = false;

         static float aspectRatio;
         static float hor3DScale;

         static void updateValues() {
            aspectRatio = (float)clientRect.right / (float)clientRect.bottom;
            hor3DScale  = 1.0f / (aspectRatio / (4.0f / 3.0f));

            // Autosculpt scaling
            Memory::openMemoryAccess(0x8AE8F8, 4);
            *(float*)0x8AE8F8 = 480.0f * aspectRatio;
            Memory::restoreMemoryAccess();

            // Arrest blur
            Memory::openMemoryAccess(0x8AFA08, 4);
            *(float*)0x8AFA08 = (1.0f / 640.0f) * ((4.0f / 3.0f) / aspectRatio);
            Memory::restoreMemoryAccess();

            // HudScale X
            Memory::openMemoryAccess(0x8AF9A4, 4);
            float hudScaleX = *(float*)0x8AF9A4 = (1.0f / (float)clientRect.right * ((float)clientRect.bottom / 480.0f)) * 2.0f;
            Memory::restoreMemoryAccess();

            // HudPos X
            Memory::openMemoryAccess(0x56FED4, 4);
            float hudPosX = *(float*)0x56FED4 = 640.0f / (640.0f * hudScaleX);
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x584EF6, 4);
            *(float*)0x584EF6 = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x599E80, 4);
            *(float*)0x599E80 = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x59A127, 4);
            *(float*)0x59A127 = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x59A5B2, 4);
            *(float*)0x59A5B2 = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x59A845, 4);
            *(float*)0x59A845 = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x5A44CC, 4);
            *(float*)0x5A44CC = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x894B40, 4);
            *(float*)0x894B40 = hudPosX;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x6E70C0, 4);
            *(float*)0x6E70C0 = hudPosX - 320.0f + 450.0f;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x6E70FF, 4);
            *(float*)0x6E70FF = hudPosX - 320.0f + 450.0f;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x6E70D3, 4);
            *(float*)0x6E70D3 = hudPosX - 320.0f + 190.0f;
            Memory::restoreMemoryAccess();

            Memory::openMemoryAccess(0x6E70E9, 4);
            *(float*)0x6E70E9 = hudPosX - 320.0f + 190.0f;
            Memory::restoreMemoryAccess();

            // Jesus christ, why does this game not have a const variable for this.
         }

         // Missing raindroplets hook
         namespace originalWSFixCode {
            static float ver3DScale = 0.75f;
            static float mirrorScale = 0.4f;
            static float f1234 = 1.25f;
            static float f06 = 0.6f;
            static float f1 = 1.0f; // horizontal for vehicle reflection
            static float flt1 = 0.0f;
            static float flt2 = 0.0f;
            static float flt3 = 0.0f;

            static const WORD dx = 16400;

            static void hkFovFix() {
               static DWORD dummyVar = 0;
               __asm {
                  mov eax, [edi + 0x60]
                  mov[dummyVar], ecx
               }
               if (dummyVar == 1 || dummyVar == 4) { //Headlights stretching, reflections etc
                  flt1 = hor3DScale;
                  flt2 = f06;
                  flt3 = f1234;
               }
               else {
                  if (dummyVar > 10) {
                     flt1 = f1;
                     flt2 = 0.5f;
                     flt3 = 1.0f;
                     _asm fld ds : f1
                     return;
                  }
                  else {
                     flt1 = 1.0f;
                     flt2 = 0.5f;
                     flt3 = 1.0f;
                  }
               }


               if (dummyVar == 3) //if rearview mirror
                  _asm fld ds : mirrorScale
               else
                  _asm fld ds : ver3DScale
            }

            static void Init() {
               Memory::writeCall(0x2CF4EA, (DWORD)hkFovFix, false);
               Memory::writeRaw(0x2CF4EF, false, 4, 0x90, 0x83, 0xF9, 0x01); // cmp ecx, 1
               Memory::writeNop(0x2CF4F3, 3, false);

               Memory::openMemoryAccess(0x6CF568, 4);
               *(DWORD*)0x6CF568 = (DWORD)&flt1;
               Memory::restoreMemoryAccess();

               Memory::openMemoryAccess(0x6CF57A, 4);
               *(DWORD*)0x6CF57A = (DWORD)&flt2;
               Memory::restoreMemoryAccess();

               Memory::openMemoryAccess(0x6CF5DE, 4);
               *(DWORD*)0x6CF5DE = (DWORD)&flt3;
               Memory::restoreMemoryAccess();

               Memory::openMemoryAccess(0x6DA8B1, 4);
               *(DWORD*)0x6DA8B1 = (DWORD)&dx;
               Memory::restoreMemoryAccess();

               // Restore missing geometry
               //Memory::writeRaw(0x2C69A7, false, 2, 0xB0, 0x00); // mov al, 0
               //Memory::writeNop(0x2C69A9, 3, false);
            }
         }

         static LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            // TODO: Add hotkey to enable/disable windowed
            if (properWindowStyle && allowWindowResizing) {
               switch (uMsg) {
                  case WM_SIZE:
                     if (!forceReset) {
                        clientRect.right  = LOWORD(lParam);
                        clientRect.bottom = HIWORD(lParam);
                        updateValues();

                        int currentResIndex = *(int*)Memory::makeAbsolute(0x50181C);
                        DWORD* newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2870);
                        Memory::openMemoryAccess(newResolutionSetupAddrs[currentResIndex] + 0x0A, 0x8);
                        *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x0A) = (int)clientRect.right;
                        *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10) = (int)clientRect.bottom;
                        Memory::restoreMemoryAccess();

                        newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2950);
                        Memory::openMemoryAccess(newResolutionSetupAddrs[currentResIndex] + 0x0A, 0x8);
                        *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x0A) = (int)clientRect.right;
                        *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10) = (int)clientRect.bottom;
                        Memory::restoreMemoryAccess();

                        Memory::openMemoryAccess(0x8AE8F8, 4);
                        *(float*)0x8AE8F8 = 480.0f * aspectRatio;
                        Memory::restoreMemoryAccess();

                        //forceReset = true;
                     }
                     return TRUE;
               }
            }
            return FALSE;
         }

         static void WINAPI beforeReset(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS* pPresentationParameters) {
            //static char mChar[128];
            //itoa(pPresentationParameters->BackBufferWidth, mChar, 10);
            //MessageBox(NULL, mChar, "", 0);
            //itoa(pPresentationParameters->BackBufferHeight, mChar, 10);
            //MessageBox(NULL, mChar, "", 0);
            //pPresentationParameters->BackBufferWidth  = clientRect.right;
            //pPresentationParameters->BackBufferHeight = clientRect.bottom;
            //sitoa(pPresentationParameters->BackBufferWidth, mChar, 10);
            //sMessageBox(NULL, mChar, "", 0);
            //sitoa(pPresentationParameters->BackBufferHeight, mChar, 10);
            //sMessageBox(NULL, mChar, "", 0);

            forceReset = false;
            Beep(500, 100);
         }
         static HRESULT WINAPI testCooperativeLevel(LPDIRECT3DDEVICE9) {
            if (forceReset) {
               return D3DERR_DEVICENOTRESET;
            }
            return FALSE;
         }

         static void Init() {
            if (properWindowStyle) {
               if (*(bool*)Memory::makeAbsolute(0x53E84C)) {
                  RECT o_cRect, n_cRect, n_wRect;
                  GetClientRect(Hooks::WndProc::windowHandle, &o_cRect);

                  // Change style
                  DWORD wStyle = GetWindowLongPtr(Hooks::WndProc::windowHandle, GWL_STYLE) | WS_OVERLAPPEDWINDOW;
                  if (!allowWindowResizing)
                     wStyle &= (~WS_SIZEBOX & ~WS_MAXIMIZEBOX);
                  SetWindowLongPtr(Hooks::WndProc::windowHandle, GWL_STYLE, wStyle);
                  // Refresh window
                  SetWindowPos(Hooks::WndProc::windowHandle, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);

                  // Correct window<->client size diff
                  GetWindowRect(Hooks::WndProc::windowHandle, &n_wRect);
                  GetClientRect(Hooks::WndProc::windowHandle, &n_cRect);
                  int n_wWidth  = n_wRect.right - n_wRect.left;
                  int n_wHeight = n_wRect.bottom - n_wRect.top;
                  int dif_wWidth  = o_cRect.right - n_cRect.right;
                  int dif_wHeight = o_cRect.bottom - n_cRect.bottom;
                  int newWidth  = n_wWidth + dif_wWidth;
                  int newHeight = n_wHeight + dif_wHeight;
                  // Resize window
                  SetWindowPos(Hooks::WndProc::windowHandle, NULL, 0, 0, newWidth, newHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_DRAWFRAME);
               }
               if (allowWindowResizing) {
                  // TODO: Improve race condition handling
                  HANDLE wsFixHandle = nullptr;
                  while (!wsFixHandle) {
                     wsFixHandle = GetModuleHandleA("NFSMostWanted.WidescreenFix.asi");
                     Sleep(1000);
                  }

                  GetClientRect(Hooks::WndProc::windowHandle, &clientRect);
                  updateValues();
                  originalWSFixCode::Init();
                  Hooks::WndProc::addExtension(&wndProcExtension);
                  MirrorHook::D3D9::SetTestCooperativeLevelExtension(&testCooperativeLevel);
                  MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::BeforeReset, &beforeReset);
               }
            }
         }
      }
   }
}