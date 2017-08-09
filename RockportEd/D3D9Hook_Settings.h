#pragma once
#include "stdafx.h"

namespace D3D9HookSettings {
	extern bool isImguiInitialized;
	namespace Options {
		extern bool isMainWindowVisible;

		extern bool opt_CustomCamera;

		extern bool opt_CustomCarBytes;
		extern char* opt_CustomCarBytesValue;
	}
}