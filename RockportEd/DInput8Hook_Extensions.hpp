#pragma once
#include "stdafx.h"
#include "Memory.h"
#include "D3D9Hook_Settings.h"
#include "Mods.h"
#include "dinput.h"

namespace DI8Extensions {
   static bool putIntoReverse = false;
   static bool reversePedals  = false;

   inline void exKeyboard_GetDeviceState(DWORD cbData, LPVOID lpvData) {
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
               if (keys[*Mods::GameInfo::key_GearDown]) {
                  Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear = max(0, *Mods::NewHUD::gear - 1);
               }
               else if (keys[*Mods::GameInfo::key_GearUp]) {
                  Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear = max(1, *Mods::NewHUD::gear + 1);
               }
            }

            if (Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear == 0) {
               if (!reversePedals) putIntoReverse = true;
               reversePedals  = true;
            }
            else if (Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear == 1) {
               keys[*Mods::GameInfo::key_Brake] = FALSE;
            }
            else if (Mods::ThingsIHaveNoIdeaWhereToPutButAreAlsoVeryImportantIThink::newGear > 0) {
               reversePedals  = false;
            }

            if (putIntoReverse) {
               keys[*Mods::GameInfo::key_Brake] = TRUE;
               putIntoReverse = false;
            }
            else if (reversePedals) {
               BYTE brake                            = keys[*Mods::GameInfo::key_Brake];
               BYTE accel                            = keys[*Mods::GameInfo::key_Accelerate];
               keys[*Mods::GameInfo::key_Brake]      = accel;
               keys[*Mods::GameInfo::key_Accelerate] = brake;
            }
         }
      }
   }

   inline void exMouse_GetDeviceState(LPVOID lpvData) {
      DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
      if (D3D9HookSettings::blockMouse) {
         ZeroMemory(mouseState->rgbButtons, 4);
      }
      else if (*Mods::GameInfo::isGameWindowInactive) {
         ZeroMemory(lpvData, sizeof(DIMOUSESTATE));
      }
   }
}