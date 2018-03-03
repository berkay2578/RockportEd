#pragma once
#include "stdafx.h"
#include "Extensions\Extensions.h"

namespace Extensions {
   namespace InGameMenu {
      static std::string carNames[6];
      static DWORD carHashes[6];
      static int opponentThing = 0;
      static DWORD returnAddress = 0;
      static void __declspec(naked) someHook() {
         __asm {
            push edx
            mov edx, eax
            pop eax

            push ebx
            mov ebx, [opponentThing]
            inc[opponentThing]

            mov eax, [carHashes + ebx * 4]
            test eax, eax
            pop ebx
            jne __callFunction
            mov eax, edx

            __callFunction :
            mov edx, 0x5E8E60

               push eax
               call edx
               pop edx
               jmp[returnAddress]
         }
      }

      struct Debug : _BaseInGameMenuItem {
         const virtual void loadData() override {
            returnAddress = 0x61E1FD + 0x5;
            Memory::writeJMP(0x61E1FD, (DWORD)&someHook, true);
            for (int i = 0; i < 6; i++) {
               carNames[i] = "carHashes id-";
               carNames[i] += std::to_string(i);
            }
            hasLoadedData = true;
         }
         const virtual void displayMenu() override {
            ImGui::Checkbox("Debug", &isEnabled);
            if (isEnabled) {
               if (ImGui::Begin("Debug__", &isEnabled, ImGuiWindowFlags_AlwaysAutoResize)) {
                  ImGui::TextWrapped("MWOnline CarSync-- Race CarModelLoad Hook");
                  ImGui::Indent(5.0f);
                  {
                     ImGui::InputInt("Last Opponent Id", &opponentThing);
                     for (int i = 0; i < 6; i++) {
                        ImGui::InputInt(carNames[i].c_str(), (int*)&carHashes[i], 1, 100, ImGuiInputTextFlags_CharsHexadecimal);
                     }
                  }
                  ImGui::Unindent(5.0f);
               }
               ImGui::End();
            }
         }
      };
   }
}