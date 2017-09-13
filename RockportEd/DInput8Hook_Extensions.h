#pragma once

namespace DI8Extensions {
   extern bool reversePedals;

   void exKeyboard_GetDeviceState(DWORD cbData, LPVOID lpvData);
   void exMouse_GetDeviceState(LPVOID lpvData);
}