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

namespace GameInternals {
   namespace Gameplay {
      namespace Variables {
         static float* gameplaySpeed   = (float*)(0x501B1C + 0x400000);
         static int*   simulationMode  =   (int*)(0x525E90 + 0x400000);
         static float* someRpmConstant = (float*)(0x497674 + 0x400000);

         static float getSomeRpmConstant() {
            return *someRpmConstant;
         }
         static void setSomeRpmConstant(const float& newValue) {
            Memory::openMemoryAccess((DWORD)someRpmConstant, 4);
            *someRpmConstant = newValue;
            Memory::restoreMemoryAccess();
         }

         static float* getTrafficRefreshIntervalAsPointer() {
            auto pBase = (float*)Memory::readPointer(0x50DCBC, false, 5, 0x8, 0x3DC, 0x1F0, 0x0, 0x4); // +4 is SpawnTime, +10 is SpawnHighway etc.
            if (!pBase)
               return nullptr;

            return pBase;
         }
         static float getTrafficRefreshInterval() {
            auto p = getTrafficRefreshIntervalAsPointer();
            return p ? *p : -1.0f;
         }
         static void setTrafficRefreshInterval(const float& newValue) {
            auto p = getTrafficRefreshIntervalAsPointer();
            if (p) {
               *p = newValue;
            }
         }
      }
   }
}