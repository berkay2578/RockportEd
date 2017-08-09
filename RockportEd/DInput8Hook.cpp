// https://github.com/glfw/glfw/blob/master/deps/mingw/dinput.h
// https://github.com/8BitPimp/fo1_rev/blob/master/dinput.cpp
// These are __thiscalls and I'm too lazy to implement that, just showing them on stack is easier.

#include "stdafx.h"
#include "DInput8Hook.h"
#include "Memory.h"
#include <dinput.h>

const GUID keyboardGUID =
{
	0x6F1D2B61,
	0xD5A0,
	0x11CF,
	0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00
};
const GUID mouseGUID =
{
	0x6F1D2B60,
	0xD5A0,
	0x11CF,
	0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00
};

typedef LPDIRECTINPUTDEVICEA* LPPDIRECTINPUTDEVICEA;
typedef HRESULT(WINAPI* GetDeviceState_t)(HINSTANCE, DWORD, LPVOID);
typedef HRESULT(WINAPI* CreateDevice_t)(HINSTANCE, REFGUID, LPPDIRECTINPUTDEVICEA, LPUNKNOWN);
typedef HRESULT(WINAPI* DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID*, LPUNKNOWN);

namespace DInput8Hook {
	GetDeviceState_t origGetDeviceState;
	CreateDevice_t origCreateDevice;
	DirectInput8Create_t origDirectInput8Create;

	HRESULT WINAPI getDeviceStateHook(HINSTANCE hInstance, DWORD cbData, LPVOID lpvData) {
		HRESULT retOrigGetDeviceState = origGetDeviceState(hInstance, cbData, lpvData);

		/*char* pkbbuf = (char*)lpvData;
		if (pkbbuf[DIK_M]) {
			Beep(1500, 100);
			pkbbuf[DIK_M] = 0;
		}*/

		return retOrigGetDeviceState;
	}

	HRESULT WINAPI createDeviceHook(HINSTANCE hInstance, REFGUID refGUID, LPPDIRECTINPUTDEVICEA lppDirectInputDevice, LPUNKNOWN lpUnkOuter) {
		HRESULT retOrigCreateDevice = origCreateDevice(hInstance, refGUID, lppDirectInputDevice, lpUnkOuter);
		if (refGUID != keyboardGUID
			/*&& refGUID != mouseGUID*/)
			return retOrigCreateDevice;

		DWORD* inputTable = *(PDWORD*)(*lppDirectInputDevice);

		Memory::openMemoryAccess(inputTable[9], 4);
		origGetDeviceState = (GetDeviceState_t)(DWORD)inputTable[9];

		inputTable[9] = (DWORD)getDeviceStateHook;
		Memory::restoreMemoryAccess();
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