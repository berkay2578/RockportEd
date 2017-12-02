#include "stdafx.h"
#include "DI8Extension.h"
#include "Game Internals\Data\Variables.h"

#include <dinput.h>
#include MIRRORHOOK_DEFINITIONS_PATH

namespace Extensions {
   namespace DI8 {
      bool reversePedals  = false;

      void WINAPI getDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         if (cbData == 256) {
            if (/*D3D9Hook::Settings::blockKeyboard || */GameInternals::Data::Variables::isGameWindowInactive()) {
               ZeroMemory(lpvData, 256);
            }
            else {
               BYTE* keys = (BYTE*)lpvData;
               /*
               if (Mods::NewHUD::gear
                   && Mods::GameInfo::isManualTransmissionEnabled
                   && *Mods::GameInfo::isManualTransmissionEnabled
                   ) {
                  if (*Mods::NewHUD::gear == 1) {
                     keys[*Mods::GameInfo::key_Brake] = FALSE;
                     if (keys[*Mods::GameInfo::key_GearDown]) {
                        keys[*Mods::GameInfo::key_Accelerate] = FALSE;
                        keys[*Mods::GameInfo::key_Brake]      = TRUE;
                     }
                  }
                  else if (*Mods::NewHUD::gear == 0 && reversePedals) {
                     BYTE brake                            = keys[*Mods::GameInfo::key_Brake];
                     BYTE accel                            = keys[*Mods::GameInfo::key_Accelerate];
                     keys[*Mods::GameInfo::key_Brake]      = accel;
                     keys[*Mods::GameInfo::key_Accelerate] = brake;
                  }
               }
               */
            }
         }
      }
      void WINAPI getDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
         DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
         /*if (D3D9Hook::Settings::blockMouse) {
            ZeroMemory(mouseState->rgbButtons, 4);
         }
         else*/ if (GameInternals::Data::Variables::isGameWindowInactive()) {
            ZeroMemory(lpvData, sizeof(DIMOUSESTATE));
         }
      }

      DWORD WINAPI Init(LPVOID) {
         HMODULE hMirrorHook = nullptr;
         while (!hMirrorHook) {
            hMirrorHook = GetModuleHandle("MirrorHook.asi");
            Sleep(100);
         }
         while (!MirrorHook::DI8::IsReady()) {
            Sleep(100);
         }

         MirrorHook::DI8::AddExtension(
            MirrorHook::DI8::DI8Device::Keyboard,
            MirrorHook::DI8::DI8Extension::GetDeviceState,
            &getDeviceState_Keyboard);
         MirrorHook::DI8::AddExtension(
            MirrorHook::DI8::DI8Device::Mouse,
            MirrorHook::DI8::DI8Extension::GetDeviceState,
            &getDeviceState_Mouse);

         return TRUE;
      }
   }
}