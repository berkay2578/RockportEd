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
#include "..\Data\Data.h"

namespace GameInternals {
   namespace Config {
      namespace Player {
         static bool isManualTransmissionEnabled() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return false;

            return *(bool*)(*pBase + 0x91);
         }
         static void setManualTransmissionEnabled(const bool& newValue) {
            auto pBase = Data::readPointerBase1();
            if (pBase)
               *(bool*)(*pBase + 0x91) = newValue;
         }

         static bool* getIsShowGameGaugesAsPointer() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return nullptr;

            return (bool*)(*pBase + 0x80);
         }
         static bool isShowGameGauges() {
            auto pBase = getIsShowGameGaugesAsPointer();
            if (pBase)
               return *pBase;

            return false;
         }
         static void setShowGameGauges(const bool& newValue) {
            auto pBase = getIsShowGameGaugesAsPointer();
            if (pBase) {
               *pBase = newValue;
            }
         }
      }
   }
}