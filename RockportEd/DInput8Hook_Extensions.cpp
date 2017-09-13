#pragma once
#include "stdafx.h"
#include "DInput8Hook_Extensions.h"
#include "Memory.h"
#include "D3D9Hook_Settings.h"
#include "Mods.h"
#include <dinput.h>

namespace DI8Extensions {
   bool reversePedals  = false;

   void exKeyboard_GetDeviceState(DWORD cbData, LPVOID lpvData) {
      if (cbData == 256) {
         if (D3D9HookSettings::blockKeyboard || *Mods::GameInfo::isGameWindowInactive) {
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

   void exMouse_GetDeviceState(LPVOID lpvData) {
      DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
      if (D3D9HookSettings::blockMouse) {
         ZeroMemory(mouseState->rgbButtons, 4);
      }
      else if (*Mods::GameInfo::isGameWindowInactive) {
         ZeroMemory(lpvData, sizeof(DIMOUSESTATE));
      }
   }
}