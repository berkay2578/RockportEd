// https://github.com/glfw/glfw/blob/master/deps/mingw/dinput.h
// https://github.com/8BitPimp/fo1_rev/blob/master/dinput.cpp
// These are __thiscalls and I'm too lazy to implement that, just showing them on stack is easier.

#include "stdafx.h"
#include "DInput8Hook.h"
#include "Memory.h"
#include <dinput.h>

#pragma comment (lib, "dxguid.lib")

typedef LPDIRECTINPUTDEVICEA* LPPDIRECTINPUTDEVICEA;
typedef HRESULT(WINAPI* GetDeviceState_t)(HINSTANCE, DWORD, LPVOID);
typedef HRESULT(WINAPI* CreateDevice_t)(HINSTANCE, REFGUID, LPPDIRECTINPUTDEVICEA, LPUNKNOWN);
typedef HRESULT(WINAPI* DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

namespace DInput8Hook {
	GetDeviceState_t origGetDeviceState_Keyboard;
	GetDeviceState_t origGetDeviceState_Mouse;
	CreateDevice_t origCreateDevice;
	DirectInput8Create_t origDirectInput8Create;

	HRESULT WINAPI getDeviceStateHook_Keyboard(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
		HRESULT retOrigGetDeviceState = origGetDeviceState_Keyboard(hInstance, cbData, lpvData);

		if (cbData == 256) {
			char* pkbbuf = (char*)lpvData;
			if (pkbbuf[DIK_M]) {
				Beep(1500, 100);
				pkbbuf[DIK_M] = 0;
			}
		}

		return retOrigGetDeviceState;
	}
	HRESULT WINAPI getDeviceStateHook_Mouse(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
		HRESULT retOrigGetDeviceState = origGetDeviceState_Mouse(hInstance, cbData, lpvData);

		DIMOUSESTATE* mouseState = (DIMOUSESTATE*)lpvData;
		if (mouseState->rgbButtons[0])
			Beep(1500, 100);


		return retOrigGetDeviceState;
	}

	HRESULT WINAPI createDeviceHook(HINSTANCE hInstance, REFGUID refGUID, LPPDIRECTINPUTDEVICEA lppDirectInputDevice, LPUNKNOWN lpUnkOuter) {
		HRESULT retOrigCreateDevice = origCreateDevice(hInstance, refGUID, lppDirectInputDevice, lpUnkOuter);
		if (refGUID == GUID_SysKeyboard) {
			DWORD* inputTable = *(PDWORD*)(*lppDirectInputDevice);
			Memory::openMemoryAccess(inputTable[9], 4);

			origGetDeviceState_Keyboard = (GetDeviceState_t)(DWORD)inputTable[9];
			inputTable[9] = (DWORD)getDeviceStateHook_Keyboard;

			Memory::restoreMemoryAccess();
		}
		else if (refGUID == GUID_SysMouse) {
			DWORD* inputTable = *(PDWORD*)(*lppDirectInputDevice);
			Memory::openMemoryAccess(inputTable[9], 4);

			origGetDeviceState_Mouse = (GetDeviceState_t)(DWORD)inputTable[9];
			inputTable[9] = (DWORD)getDeviceStateHook_Mouse;

			Memory::restoreMemoryAccess();
		}
		return retOrigCreateDevice;
	}

	DWORD origDirectInput8CreateAddr;
	BYTE origCreateDeviceBeginBytes[6];
	BYTE detourCreateDeviceBeginBytes[6];

	HRESULT WINAPI directInput8CreateHook(HINSTANCE hInstance, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN lpUnkOuter) {
	#pragma region Call original DirectInput8Create
		Memory::openMemoryAccess(origDirectInput8CreateAddr, 6);
		memcpy_s((LPVOID)origDirectInput8CreateAddr, 6, (LPVOID)origCreateDeviceBeginBytes, 6);

		HRESULT retOrigDirectInput8Create  = origDirectInput8Create(hInstance, dwVersion, riidltf, ppvOut, lpUnkOuter);

		memcpy_s((LPVOID)origDirectInput8CreateAddr, 6, (LPVOID)detourCreateDeviceBeginBytes, 6);
		Memory::restoreMemoryAccess();
	#pragma endregion

		DWORD* createDeviceAddr = (DWORD*)(
			(*(DWORD*)(*ppvOut) /* function table */)
			+ 0x0C /* create device address offset */);

		Memory::openMemoryAccess(*createDeviceAddr, 4);
		origCreateDevice = (CreateDevice_t)*createDeviceAddr;

		*createDeviceAddr = (DWORD)createDeviceHook;
		Memory::restoreMemoryAccess();
		return retOrigDirectInput8Create;
	}

	void Init() {
		// Because Most Wanted prioritizes LoadLibrary over .import resolving, we have to load them ourselves.
		LoadLibrary(L"user32.dll");
		LoadLibrary(L"advapi32.dll");

		origDirectInput8CreateAddr = (DWORD)GetProcAddress(LoadLibrary(L"dinput8.dll"), "DirectInput8Create");
		origDirectInput8Create = (DirectInput8Create_t)origDirectInput8CreateAddr;

		memcpy_s((LPVOID)origCreateDeviceBeginBytes, 6, (LPVOID)origDirectInput8CreateAddr, 6);

		Memory::writeJMP(origDirectInput8CreateAddr, (DWORD)directInput8CreateHook);
		Memory::writeRet(origDirectInput8CreateAddr + 5);

		memcpy_s((LPVOID)detourCreateDeviceBeginBytes, 6, (LPVOID)origDirectInput8CreateAddr, 6);
	}
}