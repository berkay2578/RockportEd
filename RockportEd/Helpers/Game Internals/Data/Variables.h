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

#pragma once
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data {
      namespace Variables {
         //broken, doesn't work if the player doesn't use the first save file slot... static bool* gameplayActive     = (bool*)(0x4F40C4 + 0x400000);
         //broken, doesn't work if the player doesn't use the first save file slot... static bool* gameplayAccessed   = (bool*)(0x51CD38 + 0x400000);

         static bool*    fadeScreenOn       = (bool*)(0x51CAE4 + 0x400000); // FadeScreen::IsFadeScreenOn
         static bool*    resetGameWindow    = (bool*)(0x582C39 + 0x400000);
         static bool*    gameWindowInactive = (bool*)(0x582C50 + 0x400000);
         static bool*    drawHUD            = (bool*)(0x17CAA8 + 0x400000); // really ghetto way, this is actually an assigment in a function
         static int16_t* nosFOVWidening     = (int16_t*)(0x51112C + 0x400000);

         template <typename T>
         inline T getVariable(T* pVariable) {
            return *pVariable;
         }
         template <typename T>
         inline void setVariable(LPVOID pVariable, const T& newValue) {
            Memory::openMemoryAccess((DWORD)pVariable, sizeof(T));
            *(T*)pVariable = newValue;
            Memory::restoreMemoryAccess();
         }
      }
   }
} namespace InternalVariables = GameInternals::Data::Variables;