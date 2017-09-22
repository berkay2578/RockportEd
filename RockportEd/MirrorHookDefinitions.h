#pragma once
#include "stdafx.h"
#include <d3d9.h>

namespace MirrorHook {
   typedef HRESULT(WINAPI* fAddD3D9Extender)(int type, LPVOID extenderAddress);
   typedef HWND(WINAPI* fGetWindowHandle)();
   typedef LPDIRECT3DDEVICE9(WINAPI* fGetD3D9Device)();
   typedef bool(WINAPI* fIsReady)();
}