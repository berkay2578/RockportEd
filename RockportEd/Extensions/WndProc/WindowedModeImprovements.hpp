#pragma once
#include "stdafx.h"
// Helpers
#include "Helpers\WndProc\WndProcHook.h"
#include "Helpers\Game Internals\Data\Variables.h"
#include MIRRORHOOK_DEFINITIONS_PATH

namespace Extensions {
   namespace WndProc {
      namespace WindowedModeImprovements {
         static bool isResizing = false;

         // TODO: Question whether to add RainDroplets_X Scale
         namespace WSFixCode {
            /*
            *  MIT License
            *
            *  Copyright (c) 2017 ThirteenAG
            *
            *  Permission is hereby granted, free of charge, to any person obtaining a copy
            *  of this software and associated documentation files (the "Software"), to deal
            *  in the Software without restriction, including without limitation the rights
            *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
            *  copies of the Software, and to permit persons to whom the Software is
            *  furnished to do so, subject to the following conditions:
            *
            *  The above copyright notice and this permission notice shall be included in all
            *  copies or substantial portions of the Software.
            *
            *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
            *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
            *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
            *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
            *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
            *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
            *  SOFTWARE.
            */

            static float hor3DScale  = NULL;
            static float ver3DScale  = 0.75f;
            static float mirrorScale = 0.40f;
            static float f1234       = 1.25f;
            static float f06         = 0.60f;
            static float f1          = 1.00f; // horizontal for vehicle reflection
            static float flt1        = 0.00f;
            static float flt2        = 0.00f;
            static float flt3        = 0.00f;
            static const WORD dx     = 16400;

            static void hkFOVFix() {
               static DWORD dummyVar;
               __asm {
                  mov eax, [edi + 0x60]
                  mov[dummyVar], ecx
               }
               if (dummyVar == 1 || dummyVar == 4) { // Headlights stretching, reflections etc
                  flt1 = hor3DScale;
                  flt2 = f06;
                  flt3 = f1234;
               } else {
                  if (dummyVar > 10) {
                     flt1 = f1;
                     flt2 = 0.5f;
                     flt3 = 1.0f;
                     _asm fld ds : f1
                     return;
                  } else {
                     flt1 = 1.0f;
                     flt2 = 0.5f;
                     flt3 = 1.0f;
                  }
               }

               if (dummyVar == 3) // if rearview mirror
                  _asm fld ds : mirrorScale
               else
                  _asm fld ds : ver3DScale
            }
            static void Init() {
               Memory::writeCall(0x2CF4EA, (DWORD)hkFOVFix, false);
               Memory::writeRaw(0x2CF4EF, false, 4, 0x90, 0x83, 0xF9, 0x01);
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
            }
         }

         inline void updateValues(const float& newWidth, const float& newHeight) {
            float aspectRatio = newWidth / newHeight;

            // Resolution
            {
               int currentResIndex = *(int*)Memory::makeAbsolute(0x50181C);
               DWORD* newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2870);
               Memory::openMemoryAccess(newResolutionSetupAddrs[currentResIndex] + 0x0A, 0x8);
               *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x0A) = (int)newWidth;
               *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10) = (int)newHeight;
               Memory::restoreMemoryAccess();

               newResolutionSetupAddrs = (DWORD*)Memory::makeAbsolute(0x2C2950);
               Memory::openMemoryAccess(newResolutionSetupAddrs[currentResIndex] + 0x0A, 0x8);
               *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x0A) = (int)newWidth;
               *(int*)(newResolutionSetupAddrs[currentResIndex] + 0x10) = (int)newHeight;
               Memory::restoreMemoryAccess();
            }

            WSFixCode::hor3DScale = 1.0f / (aspectRatio / (4.0f / 3.0f));

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
            float hudScaleX = *(float*)0x8AF9A4 = (1.0f / newWidth * (newHeight / 480.0f)) * 2.0f;
            Memory::restoreMemoryAccess();

            // HudPos X
            // TODO: Improve this
            {
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
            }
         }
         static LRESULT CALLBACK wndProcExtension(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            switch (uMsg) {
               case WM_SIZE:
               {
                  updateValues((float)LOWORD(lParam), (float)HIWORD(lParam));

                  isResizing = true;
                  GameInternals::Data::Variables::setDrawHUD(false);
                  GameInternals::Data::Variables::setResetGameWindow(true);
               }
               return TRUE;
            }
            return FALSE;
         }

         static void WINAPI reset(LPVOID, LPVOID) {
            if (isResizing) {
               GameInternals::Data::Variables::setDrawHUD(true);
               isResizing = false;
            }
         }

         static void Init() {
            if (*(bool*)Memory::makeAbsolute(0x53E84C)) {
               RECT o_cRect, n_cRect, n_wRect;
               GetClientRect(Helpers::WndProcHook::windowHandle, &o_cRect);

               // Change style
               DWORD wStyle = GetWindowLongPtr(Helpers::WndProcHook::windowHandle, GWL_STYLE) | WS_OVERLAPPEDWINDOW;
               SetWindowLongPtr(Helpers::WndProcHook::windowHandle, GWL_STYLE, wStyle);
               // Refresh window
               SetWindowPos(Helpers::WndProcHook::windowHandle, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);

               // Correct window<->client size diff
               GetWindowRect(Helpers::WndProcHook::windowHandle, &n_wRect);
               GetClientRect(Helpers::WndProcHook::windowHandle, &n_cRect);
               int n_wWidth  = n_wRect.right - n_wRect.left;
               int n_wHeight = n_wRect.bottom - n_wRect.top;
               int dif_cWidth  = o_cRect.right - n_cRect.right;
               int dif_cHeight = o_cRect.bottom - n_cRect.bottom;
               int newWidth  = n_wWidth + dif_cWidth;
               int newHeight = n_wHeight + dif_cHeight;
               // Resize window
               SetWindowPos(Helpers::WndProcHook::windowHandle, HWND_TOP, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_DRAWFRAME);

               // TODO: Handle race condition with WSFix
               // Cannot trust `HANDLE asiFile = FindFirstFile("*.asi", fd);` from ThirteenAG's ASI loader because the user might not be using it
               RECT clientRect = { 0 };
               GetClientRect(Helpers::WndProcHook::windowHandle, &clientRect);
               updateValues((float)clientRect.right, (float)clientRect.bottom);
               WSFixCode::Init();

               Helpers::WndProcHook::addExtension(&wndProcExtension);
               MirrorHook::D3D9::AddExtension(MirrorHook::D3D9::D3D9Extension::AfterReset, &reset);
            }
         }
      }
   }
}