#include "stdafx.h"
#include "DI8Extension.h"
#include "Extensions\Extensions.h"
#include "Helpers\WndProc\WndProcHook.h"

namespace Extensions {
   namespace DI8 {
      void WINAPI getDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         if (cbData == 256) {
            if (InternalVariables::isGameWindowInactive() || imguiIO->WantCaptureKeyboard
                || GetForegroundWindow() != Helpers::WndProcHook::windowHandle) {
               ZeroMemory(lpvData, 256);
               return;
            }
         }
      }
      void WINAPI getDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
         if (InternalVariables::isGameWindowInactive() || imguiIO->WantCaptureMouse
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