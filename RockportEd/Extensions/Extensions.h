#pragma once
// InGameMenu
#include <d3d9.h>
#include "Helpers\imgui\imgui.h"
#include "InGame Menu\Items\_BaseInGameMenuItem.hpp"
// DirectInput8
#include <dinput.h>
// Helpers
#include "Helpers\Settings\Settings.h"
#include "Helpers\Game Internals\GameInternals.h"
#include MIRRORHOOK_DEFINITIONS_PATH

namespace Extensions {
   static ImGuiIO* imguiIO = &ImGui::GetIO();
}