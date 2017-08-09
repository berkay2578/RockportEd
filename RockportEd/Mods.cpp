#pragma once
#include "stdafx.h"
#include "Mods.h"
#include "Memory.h"
#include "D3D9Hook_Settings.h"

using std::map;

namespace Mods {
	int* activeCamera;
	map<int, map<char*, float*>> cameraData;

#pragma region Car bytes
	DWORD carbytesReadHook_Entry;
	DWORD carbytesReadHook_Ret;
	DWORD carBytes;

	void __declspec(naked) carbytesReadHook() {
		__asm {
			mov edi, [esp + 0x10]
			mov ebx, ecx
			pushad
		}
		if (D3D9HookSettings::Options::opt_CustomCarBytes) {
			/* rehook inside the loop, game loads bytes for eacH COMPONENT FUCKING FUCK YOU OLD EAGL
			__asm {
				push eax
				mov eax, [D3D9HookSettings::Options::opt_CustomCarBytesValue]
				mov[ebx], eax
				pop eax
			}*/
		}
		else {
			__asm {
				push eax
				mov eax, [ebx]
				mov[carBytes], eax
				pop eax
			}
			sprintf(D3D9HookSettings::Options::opt_CustomCarBytesValue, "%X", carBytes);
		}
		__asm {
			popad
			jmp carbytesReadHook_Ret
		}
	}
#pragma endregion

	// fix these damn pointer smh, fuck is + 0x2B148
	// read game code, there's [edi+0C] that's set from ecx for activeCam and blablablablalba
	void Init() {
	#pragma region Camera
		activeCamera = (int*)Memory::readPointer(0x51CF90, 2, 0x10, 0x8C);
		DWORD camBase = *Memory::readPointer(0x51DCC8, 1, 0xF4);

		map<char*, float*> farCamera;
		farCamera["X"] = (float*)(camBase + 0x2B148);
		farCamera["Z"] = (float*)(camBase + 0x2B168);
		farCamera["Fov"] = (float*)(camBase + 0x2B158);
		farCamera["HorAngle"] = (float*)(camBase + 0x2B138);
		farCamera["VerAngle"] = (float*)(camBase + 0x2B178);

		map<char*, float*> nearCamera;
		nearCamera["X"] = (float*)(camBase + 0x2B058);
		nearCamera["Z"] = (float*)(camBase + 0x2B078);
		nearCamera["Fov"] = (float*)(camBase + 0x2B068);
		nearCamera["HorAngle"] = (float*)(camBase + 0x2B048);
		nearCamera["VerAngle"] = (float*)(camBase + 0x2B088);

		map<char*, float*> bumperCamera;
		bumperCamera["X"] = (float*)(camBase + 0x2AFE0);
		bumperCamera["Z"] = (float*)(camBase + 0x2B000);
		bumperCamera["Fov"] = (float*)(camBase + 0x2AFF0);
		bumperCamera["HorAngle"] = (float*)(camBase + 0x2AFD0);
		bumperCamera["VerAngle"] = (float*)(camBase + 0x2B010);

		cameraData[3] = farCamera;
		cameraData[2] = nearCamera;
		cameraData[0] = bumperCamera;
	#pragma endregion

		carbytesReadHook_Entry = Memory::makeAbsolute(0x16F2B3);
		carbytesReadHook_Ret = Memory::makeAbsolute(0x16F2B9);
		Memory::writeJMP(carbytesReadHook_Entry, (DWORD)carbytesReadHook);
		Memory::writeNop(carbytesReadHook_Entry + 0x5, 1);
	}
}