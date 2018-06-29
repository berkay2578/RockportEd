/*
   MIT License
   
   Copyright (c) Berkay Yigit
   
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
#include "DI8Extension.h"
#include "Extensions\Extensions.h"
#include "Helpers\WndProc\WndProcHook.h"

namespace Extensions {
   namespace DI8 {
      void WINAPI getDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         if (cbData == 256) {
            if (InternalVariables::getVariable(InternalVariables::gameWindowInactive) || imguiIO->WantCaptureKeyboard
                || GetForegroundWindow() != Helpers::WndProcHook::windowHandle) {
               ZeroMemory(lpvData, 256);
               return;
            }
         }
      }
      void WINAPI getDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
         if (InternalVariables::getVariable(InternalVariables::gameWindowInactive) || imguiIO->WantCaptureMouse
             || GetForegroundWindow() != Helpers::WndProcHook::windowHandle) {
            ZeroMemory(mouseState->rgbButtons, sizeof(mouseState->rgbButtons));
         }
      }

      void Init() {
         MirrorHook::DI8::AddExtension(
            MirrorHook::DI8::DI8Device::Keyboard,
            MirrorHook::DI8::DI8Extension::GetDeviceState,
            &getDeviceState_Keyboard);
         MirrorHook::DI8::AddExtension(
            MirrorHook::DI8::DI8Device::Mouse,
            MirrorHook::DI8::DI8Extension::GetDeviceState,
            &getDeviceState_Mouse);
      }
   }
}