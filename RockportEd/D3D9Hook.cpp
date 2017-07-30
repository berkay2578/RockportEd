#include "stdafx.h"
#include "D3D9Hook.h"
#include "VTableHook.hpp"
#include "Settings.h"
#include "D3D9Hook_Settings.h"
#include "Memory.h"
#include "Mods.h"

#include <d3d9.h>
#include <d3dx9.h>

typedef HRESULT(WINAPI* Reset_t)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI* BeginScene_t)(LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* EndScene_t)(LPDIRECT3DDEVICE9);
typedef HRESULT(WINAPI* BeginStateBlock_t)(LPDIRECT3DDEVICE9);

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

#include "imgui/imgui.h"
#include "imgui/dx9/imgui_impl_dx9.h"
#include "imgui/extra_fonts/RobotoMedium.hpp"
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <memory>
using std::unique_ptr;
using std::make_unique;

//SetWindowSubClass
#include <commctrl.h>

namespace D3D9Hook {
	HWND windowHandle = nullptr;
	WNDPROC origWndProc;
	DWORD d3dDeviceAddress;
	LPDIRECT3DDEVICE9 d3dDevice = nullptr;
	unique_ptr<VTableHook> d3dDeviceHook = nullptr;

	Reset_t origReset = nullptr;
	HRESULT WINAPI resetHook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	BeginScene_t origBeginScene = nullptr;
	HRESULT WINAPI beginSceneHook(LPDIRECT3DDEVICE9 pDevice);
	EndScene_t origEndScene = nullptr;
	HRESULT WINAPI endSceneHook(LPDIRECT3DDEVICE9 pDevice);
	BeginStateBlock_t origBeginStateBlock = nullptr;
	HRESULT WINAPI beginStateBlockHook(LPDIRECT3DDEVICE9 pDevice);

	bool showUserGuide = false;

	void doImGuiStyle() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.GrabRounding = 2.0f;
		style.GrabMinSize = 7.0f;
		style.WindowRounding = 0.f;
		style.ScrollbarRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.FramePadding = ImVec2(5.0f, 2.0f);
		style.ItemSpacing = ImVec2(8.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(9.0f, 3.0f);
		style.IndentSpacing = 22.0f;
		style.ScrollbarSize = 16.0f;
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowPadding = ImVec2(14.0f, 14.0f);
		style.WindowMinSize = ImVec2(250.0f, 120.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
		style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
		style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
	}

	HRESULT WINAPI beginSceneHook(LPDIRECT3DDEVICE9 pDevice) {
		if (!D3D9HookSettings::isImguiInitialized) {
			ImGui_ImplDX9_Init(windowHandle, d3dDevice);
			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = NULL;
			io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium::RobotoMedium_compressed_data, RobotoMedium::RobotoMedium_compressed_size, 14.0f);
			io.FontDefault = NULL;
			D3D9HookSettings::isImguiInitialized = true;
		}

		ImGui_ImplDX9_NewFrame();
		doImGuiStyle();
		return origBeginScene(pDevice);
	}
	HRESULT WINAPI endSceneHook(LPDIRECT3DDEVICE9 pDevice) {
		if (pDevice->TestCooperativeLevel() == D3D_OK) {
			if (D3D9HookSettings::isImguiInitialized) {
				if (D3D9HookSettings::Options::isMainWindowVisible) {
					if (showUserGuide) {
						ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
						ImGui::Begin("User Guide", &showUserGuide, ImVec2(0.0f, 0.0f), 1.0f,
									 ImGuiWindowFlags_NoSavedSettings);
						ImGui::BulletText("Press Insert to show/hide RockportEd.");
						ImGui::BulletText("Double-click on title bar to collapse windows.");
						ImGui::BulletText("Click and drag on lower right corner to resize windows.");
						ImGui::BulletText("Click and drag on any empty space to move windows.");
						ImGui::BulletText("Mouse Wheel to scroll.");
						ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
						ImGui::BulletText("CTRL+Click on a slider to input text.");
						ImGui::BulletText(
							"While editing text:\n"
							"- Hold SHIFT+Left/Right or use mouse to select text\n"
							"- CTRL+Left/Right to word jump\n"
							"- CTRL+A or double-click to select all\n"
							"- CTRL+X, CTRL+C, CTRL+V clipboard\n"
							"- CTRL+Z, CTRL+Y to undo/redo\n"
							"- ESCAPE to cancel\n"
							"- You can apply arithmetic operators +,*,/ on numerical values.\n");
						ImGui::BulletText("Click on the button at the top-right of this window to close it.");
						ImGui::End();
					}

					ImGui::SetNextWindowPos(ImVec2(60, 60), ImGuiSetCond_Once);
					ImGui::Begin("RockportEd", (bool*)0, ImVec2(250.0f, 120.0f), 0.9f,
								 ImGuiWindowFlags_NoSavedSettings);

					ImGui::Checkbox("Debug Camera", &D3D9HookSettings::Options::opt_DebugCamera);
					ImGui::Checkbox("Custom Camera", &D3D9HookSettings::Options::opt_CustomCamera);
					if (D3D9HookSettings::Options::opt_CustomCamera) {
						ImGui::BeginChild("##CustomCamera", ImVec2(0.0f, 0.0f), true);
						ImGui::TextWrapped("X");
						ImGui::SliderFloat("##CameraX", &D3D9HookSettings::Options::opt_CustomCamera_X, -5.0f, 5.0f);
						ImGui::TextWrapped("Horizontal Angle");
						ImGui::SliderFloat("##CameraHorAngle", &D3D9HookSettings::Options::opt_CustomCamera_HorAngle, -5.0f, 5.0f);
						ImGui::TextWrapped("Z");
						ImGui::SliderFloat("##CameraZ", &D3D9HookSettings::Options::opt_CustomCamera_Z, 0.0f, 5.0f);
						ImGui::TextWrapped("FOV");
						ImGui::SliderFloat("##CameraFov", &D3D9HookSettings::Options::opt_CustomCamera_Fov, 10.0f, 100.0f);
						ImGui::TextWrapped("Rotation");
						ImGui::SliderAngle("##CameraRot", &D3D9HookSettings::Options::opt_CustomCamera_Rot, 0.0f, 90.0f);
						ImGui::EndChild();
					}

					ImGui::End();
				}

				ImGui::Render();
			}
		}

		return origEndScene(pDevice);
	}
	HRESULT WINAPI resetHook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
		if (!D3D9HookSettings::isImguiInitialized)
			return origReset(pDevice, pPresentationParameters);

		ImGui_ImplDX9_InvalidateDeviceObjects();
		auto retOrigReset = origReset(pDevice, pPresentationParameters);

		if (!pDevice || pDevice->TestCooperativeLevel() != D3D_OK) {
			ImGui_ImplDX9_Shutdown();
			D3D9HookSettings::isImguiInitialized = false;

			return retOrigReset;
		}

		ImGui_ImplDX9_CreateDeviceObjects();
		return retOrigReset;
	}
	HRESULT WINAPI beginStateBlockHook(LPDIRECT3DDEVICE9 pDevice) {
		d3dDeviceHook->UnhookAll();

		auto retBeginStateBlock = origBeginStateBlock(pDevice);

		origReset = d3dDeviceHook->Hook(16, resetHook);
		origBeginScene = d3dDeviceHook->Hook(41, beginSceneHook);
		origEndScene = d3dDeviceHook->Hook(42, endSceneHook);
		origBeginStateBlock = d3dDeviceHook->Hook(60, beginStateBlockHook);
		return retBeginStateBlock;
	}

	LRESULT CALLBACK WndProcHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_KEYMENU)
			return true;

		if (D3D9HookSettings::isImguiInitialized) {
			ImGuiIO io = ImGui::GetIO();

			if (uMsg == WM_KEYUP) {
				if (wParam == VK_INSERT && !(io.WantTextInput | io.WantCaptureKeyboard))
					D3D9HookSettings::Options::isMainWindowVisible = !D3D9HookSettings::Options::isMainWindowVisible;
			}

			if (D3D9HookSettings::Options::isMainWindowVisible) {
				ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam);

				if (io.WantCaptureMouse) {
					switch (uMsg) {
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
						case WM_MBUTTONDOWN:
						case WM_MBUTTONUP:
						case WM_RBUTTONDOWN:
						case WM_RBUTTONUP:
							return true;
					}
				}
				if (io.WantTextInput) {
					return true;
				}
			}
		}

		return CallWindowProc(origWndProc, hWnd, uMsg, wParam, lParam);
	}

	void Init() {
		d3dDeviceAddress = *(DWORD*)Memory::makeAbsolute(0x582BDC);
		d3dDevice = (LPDIRECT3DDEVICE9)d3dDeviceAddress;

		D3DDEVICE_CREATION_PARAMETERS cParams;
		d3dDevice->GetCreationParameters(&cParams);
		windowHandle = cParams.hFocusWindow;

		origWndProc = (WNDPROC)SetWindowLongPtr(windowHandle, GWL_WNDPROC, (LONG_PTR)&WndProcHook);

		// fix double clicks
		DWORD Style = GetClassLong(windowHandle, GCL_STYLE) & ~CS_DBLCLKS;
		SetClassLong(windowHandle, GCL_STYLE, Style);

		d3dDeviceHook = make_unique<VTableHook>((PDWORD*)d3dDevice);
		origReset = d3dDeviceHook->Hook(16, resetHook);
		origBeginScene = d3dDeviceHook->Hook(41, beginSceneHook);
		origEndScene = d3dDeviceHook->Hook(42, endSceneHook);
		origBeginStateBlock = d3dDeviceHook->Hook(60, beginStateBlockHook);

		showUserGuide = Settings::isFirstTime();
	}
}