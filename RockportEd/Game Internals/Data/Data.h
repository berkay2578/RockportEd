#pragma once
#include "Variables.h"
#include "..\..\Memory.h"

namespace GameInternals {
   namespace Data {
      static DWORD* readPointerBase1() {
         return Memory::readPointer(0x51CF90, 1, 0x10);
      }
      static DWORD* readPointerBase2() { // vtable of many floats
         return Memory::readPointer(0x513E80, 2, 0x8, 0x60);
      }
      static DWORD* getCarPowerBase() {
         return Memory::readPointer(0x5142D0, 1, 0x20);
      }
      static DWORD* getCameraBase() {
         return Memory::readPointer(0x51DCC8, 1, 0xF4);
      }
   }
}