#include "stdafx.h"
#include "DInput8Hook.h"
#include "DInput8Hook_Extensions.h"
#include "Memory.h"
#include <dinput.h>

typedef LPDIRECTINPUTDEVICEA* LPPDIRECTINPUTDEVICEA;
typedef HRESULT(WINAPI* GetDeviceState_t)(HINSTANCE, DWORD, LPVOID);
typedef HRESULT(WINAPI* CreateDevice_t)(HINSTANCE, REFGUID, LPPDIRECTINPUTDEVICEA, LPUNKNOWN);
typedef HRESULT(WINAPI* DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

namespace DInput8Hook {
   GetDeviceState_t     origGetDeviceState_Keyboard = nullptr;
   GetDeviceState_t     origGetDeviceState_Mouse    = nullptr;
   CreateDevice_t       origCreateDevice            = nullptr;
   DirectInput8Create_t origDirectInput8Create      = nullptr;


   HRESULT WINAPI hkGetDeviceState_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
      HRESULT retOrigGetDeviceState = origGetDeviceState_Keyboard(hInstance, cbData, lpvData);

      DI8Extensions::exKeyboard_GetDeviceState(cbData, lpvData);
      return retOrigGetDeviceState;
   }
   HRESULT WINAPI hkGetDeviceState_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
      HRESULT retOrigGetDeviceState = origGetDeviceState_Mouse(hInstance, cbData, lpvData);

      DI8Extensions::exMouse_GetDeviceState(lpvData);
      return retOrigGetDeviceState;
   }

   HRESULT WINAPI hkCreateDevice(HINSTANCE hInstance, REFGUID refGUID, LPPDIRECTINPUTDEVICEA lppDirectInputDevice, LPUNKNOWN lpUnkOuter) {
      HRESULT retOrigCreateDevice = origCreateDevice(hInstance, refGUID, lppDirectInputDevice, lpUnkOuter);
      if (refGUID == GUID_SysKeyboard) {
         DWORD* inputTable = *(PDWORD*)(*lppDirectInputDevice);
         Memory::openMemoryAccess(inputTable[9], 4);

         origGetDeviceState_Keyboard = (GetDeviceState_t)(DWORD)inputTable[9];
         inputTable[9]               = (DWORD)hkGetDeviceState_Keyboard;

         Memory::restoreMemoryAccess();
      }
      else if (refGUID == GUID_SysMouse) {
         DWORD* inputTable = *(PDWORD*)(*lppDirectInputDevice);
         Memory::openMemoryAccess(inputTable[9], 4);

         origGetDeviceState_Mouse = (GetDeviceState_t)(DWORD)inputTable[9];
         inputTable[9]            = (DWORD)hkGetDeviceState_Mouse;

         Memory::restoreMemoryAccess();
      }

      return retOrigCreateDevice;
   }

   DWORD origDirectInput8CreateAddr;
   BYTE origCreateDeviceBeginBytes[6];
   BYTE detourCreateDeviceBeginBytes[6];

   HRESULT WINAPI hkDirectInput8Create(HINSTANCE hInstance, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN lpUnkOuter) {
      // Call original DirectInput8Create
      Memory::openMemoryAccess(origDirectInput8CreateAddr, 6);
      memcpy_s((LPVOID)origDirectInput8CreateAddr, 6, (LPVOID)origCreateDeviceBeginBytes, 6);

      HRESULT retOrigDirectInput8Create = origDirectInput8Create(hInstance, dwVersion, riidltf, ppvOut, lpUnkOuter);

      memcpy_s((LPVOID)origDirectInput8CreateAddr, 6, (LPVOID)detourCreateDeviceBeginBytes, 6);
      Memory::restoreMemoryAccess();

      DWORD* functionTable = (DWORD*)(**(PDWORD*)ppvOut);
      if (!origCreateDevice)
         origCreateDevice = (CreateDevice_t)functionTable[3];

      Memory::openMemoryAccess(functionTable[3], 4);
      functionTable[3] = (DWORD)hkCreateDevice;
      Memory::restoreMemoryAccess();
      return retOrigDirectInput8Create;
   }

   void Init() {
      origDirectInput8CreateAddr = (DWORD)GetProcAddress(LoadLibrary(L"dinput8.dll"), "DirectInput8Create");
      origDirectInput8Create     = (DirectInput8Create_t)origDirectInput8CreateAddr;

      memcpy_s((LPVOID)origCreateDeviceBeginBytes, 6, (LPVOID)origDirectInput8CreateAddr, 6);

      Memory::writeJMP(origDirectInput8CreateAddr, (DWORD)hkDirectInput8Create);
      Memory::writeRet(origDirectInput8CreateAddr + 5);

      memcpy_s((LPVOID)detourCreateDeviceBeginBytes, 6, (LPVOID)origDirectInput8CreateAddr, 6);
   }
}