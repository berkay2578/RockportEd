#include "stdafx.h"
#include "Extensions\Extensions.h"
#include "DI8Extension.h"

namespace Extensions {
   namespace DI8 {
      void WINAPI getDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         if (cbData == 256) {
            if (GameInternals::Data::Variables::isGameWindowInactive() || imguiIO->WantTextInput/* || imguiIO->WantCaptureKeyboard*/) {
               ZeroMemory(lpvData, 256);
            }
         }
      }
      void WINAPI getDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
         if (GameInternals::Data::Variables::isGameWindowInactive() || imguiIO->WantCaptureMouse) {
            ZeroMemory(lpvData, sizeof(DIMOUSESTATE));
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