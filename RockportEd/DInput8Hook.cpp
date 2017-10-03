#include "stdafx.h"
#include "DInput8Hook.h"
#include "D3D9Hook.h"
#include "Mods.h"

#include <dinput.h>
#include MIRRORHOOK_DEFINITIONS_PATH

namespace DInput8Hook {
   bool reversePedals  = false;

   void WINAPI getDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
      if (cbData == 256) {
         if (D3D9Hook::Settings::blockKeyboard || *Mods::GameInfo::isGameWindowInactive) {
            ZeroMemory(lpvData, 256);
         }
         else {
            BYTE* keys = (BYTE*)lpvData;

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

         }
      }
   }
   void WINAPI getDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
      DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
      if (D3D9Hook::Settings::blockMouse) {
         ZeroMemory(mouseState->rgbButtons, 4);
      }
      else if (*Mods::GameInfo::isGameWindowInactive) {
         ZeroMemory(lpvData, sizeof(DIMOUSESTATE));
      }
   }

   DWORD WINAPI Init(LPVOID) {
      HMODULE hMirrorHook = nullptr;
      while (!hMirrorHook) {
         hMirrorHook = GetModuleHandle("MirrorHook.asi");
         Sleep(100);
      }

      while (!MirrorHook::DirectInput8::IsReady()) {
         Sleep(100);
      }

      MirrorHook::DirectInput8::AddDirectInput8Extender(
         MirrorHook::DirectInput8::DirectInput8Device::Keyboard,
         MirrorHook::DirectInput8::DirectInput8Extender::GetDeviceState,
         &getDeviceState_Keyboard);
      MirrorHook::DirectInput8::AddDirectInput8Extender(
         MirrorHook::DirectInput8::DirectInput8Device::Mouse,
         MirrorHook::DirectInput8::DirectInput8Extender::GetDeviceState,
         &getDeviceState_Mouse);

      return TRUE;
   }
}