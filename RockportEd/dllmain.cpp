/*
   MIT License
   
   Copyright (c) Berkay Yigit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include "stdafx.h"
// Helpers
#include "Helpers\Game Internals\GameInternals.h"
#include "Helpers\Settings\Settings.h"
#include "Helpers\WndProc\WndProcHook.h"
// Extensions
#include "Extensions\InGame Menu\InGameMenu.h"
#include "Extensions\InGame Menu\Items\_LoadInGameMenuItems.hpp"
#include "Extensions\DInput8\DI8Extension.h"
#include "Extensions\WndProc\WndProcExtensions.hpp"

DWORD WINAPI Init(LPVOID) {
   // MirrorHook
   HMODULE hMirrorHook = nullptr;
   while (!hMirrorHook) {
      hMirrorHook = GetModuleHandleA("MirrorHook.asi");
      Sleep(100);
   }
   MirrorHook::PrepareFor(MirrorHook::Game::MostWanted);

   // Helpers
   Memory::Init();
   Settings::loadSettings();
   Helpers::WndProcHook::Init();

   // Extensions
   Extensions::WndProc::InitAll();
   Extensions::InGameMenu::Init();
   while (!Extensions::imguiIO)
      Sleep(10);
   Extensions::DI8::Init();

   // InGameMenu Items
   Extensions::InGameMenu::loadItemsToInGameMenu();

   static std::string copyright = "Copyright (c) 2018 Berkay Yiðit - berkay(2578). Greetz to GamerZ, still learning from him somehow. SpeedyHeart is honestly the most annoying, egomaniac person I've ever had contact with, may god help them.";
   return TRUE;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID) {
   if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
      DisableThreadLibraryCalls(hModule);
      CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&Init, NULL, 0, NULL);
   }
   return TRUE;
}