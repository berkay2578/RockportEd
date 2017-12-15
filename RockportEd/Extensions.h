#pragma once
// InGameMenu
#include <d3d9.h>
#include "imgui\imgui.h"
#include "_BaseInGameMenuItem.hpp"
// DirectInput8
#include <dinput.h>
// Helpers
#include "Settings.h"
#include "Game Internals\GameInternals.h"
#include MIRRORHOOK_DEFINITIONS_PATH

namespace Extensions {
   static ImGuiIO* imguiIO = &ImGui::GetIO();
}