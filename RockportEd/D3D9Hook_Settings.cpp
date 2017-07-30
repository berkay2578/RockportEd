#include "stdafx.h"
#include "D3D9Hook_Settings.h"

namespace D3D9HookSettings {
	bool isImguiInitialized = false;
	namespace Options {
		bool isMainWindowVisible = true;

		bool opt_DebugCamera = false;

		bool opt_CustomCamera = false;
		float opt_CustomCamera_X = 3.0f;
		float opt_CustomCamera_HorAngle = 3.0f;
		float opt_CustomCamera_Z = 3.0f;
		float opt_CustomCamera_Fov = 3.0f;
		float opt_CustomCamera_Rot = 3.0f;
	}
}