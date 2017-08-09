#include "stdafx.h"
#include "D3D9Hook_Settings.h"

namespace D3D9HookSettings {
	bool isImguiInitialized = false;
	namespace Options {
		bool isMainWindowVisible = true;

		bool opt_CustomCamera = false;

		bool opt_CustomCarBytes = false;
		char* opt_CustomCarBytesValue = new char[8];
	}
}