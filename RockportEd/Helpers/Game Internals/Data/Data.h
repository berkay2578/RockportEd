#pragma once
#include <WinDef.h>
#include "Variables.h"
#include "Helpers\Memory\Memory.h"

namespace GameInternals {
   namespace Data {
      static DWORD* readPointerBase1() {
         return Memory::readPointer(0x51CF90, 1, 0x10);
      }
      static DWORD* readPointerBase2() { // vtable of many floats
         return Memory::readPointer(0x513E80, 2, 0x8, 0x60);
      }
   }
}