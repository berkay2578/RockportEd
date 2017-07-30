#pragma once
#include "stdafx.h"

namespace D3D9HookSettings {
	extern bool isImguiInitialized;
	namespace Options {
		extern bool isMainWindowVisible;

		extern bool opt_DebugCamera;

		extern bool opt_CustomCamera;
		extern float opt_CustomCamera_X;
		extern float opt_CustomCamera_HorAngle;
		extern float opt_CustomCamera_Z;
		extern float opt_CustomCamera_Fov;
		extern float opt_CustomCamera_Rot;
	}
}