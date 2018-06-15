#include "stdafx.h"
#include "Memory.h"

namespace Memory {
   CRITICAL_SECTION cs;
   DWORD oldMemoryAccess;
   DWORD memoryAccessAddress;
   int memoryAccessSize;
   DWORD baseAddress = 0;

   void openMemoryAccess(const DWORD& address, const int& size) {
      memoryAccessAddress = address;
      memoryAccessSize = size;
      VirtualProtect((LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldMemoryAccess);
   }
   void restoreMemoryAccess() {
      VirtualProtect((LPVOID)memoryAccessAddress, memoryAccessSize, oldMemoryAccess, &oldMemoryAccess);
      memoryAccessAddress = 0;
      memoryAccessSize = 0;
   }

   DWORD calculateRelativeAddress(const DWORD& from, const DWORD& to, const bool& isFromAbsolute) {
      if (!isFromAbsolute) {
         DWORD absoluteFrom = baseAddress + from;
         return to - absoluteFrom - 0x5;
      }
      return to - from - 0x5;
   }

   DWORD makeAbsolute(const DWORD& relativeAddress) {
      return baseAddress + relativeAddress;
   }

   DWORD* readPointer(const DWORD& baseOffset, const bool& isBaseOffsetAbsolute, const int offsetCount, ...) {
      DWORD* pointer = (DWORD*)baseOffset;
      if (!isBaseOffsetAbsolute)
         pointer = (DWORD*)makeAbsolute(baseOffset);
      if (!*pointer)
         return nullptr;

      if (offsetCount > 0) {
         va_list offsets;
         va_start(offsets, offsetCount);
         for (int i = 0; i < offsetCount - 1; i++) {
            if (!pointer)
               break;

            pointer = (DWORD*)(*pointer + va_arg(offsets, int));
            if (!*pointer) {
               pointer = nullptr;
               break;
            }
         }

         if (pointer)
            pointer = (DWORD*)(*pointer + va_arg(offsets, int));
         va_end(offsets);
      }

      return pointer;
   }

   void writeCall(const DWORD& from, const DWORD& to, const bool& isFromAbsolute) {
      EnterCriticalSection(&cs);
      DWORD relativeAddress = calculateRelativeAddress(from, to, isFromAbsolute);

      if (!isFromAbsolute) {
         DWORD absoluteFrom = baseAddress + from;

         openMemoryAccess(absoluteFrom, 5);
         *(BYTE*)(absoluteFrom) = 0xE8;
         *(DWORD*)(absoluteFrom + 0x1) = relativeAddress;
      } else {
         openMemoryAccess(from, 5);
         *(BYTE*)(from) = 0xE8;
         *(DWORD*)(from + 0x1) = relativeAddress;
      }

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   void writeJMP(const DWORD& from, const DWORD& to, const bool& isFromAbsolute) {
      EnterCriticalSection(&cs);
      DWORD relativeAddress = calculateRelativeAddress(from, to, isFromAbsolute);

      if (!isFromAbsolute) {
         DWORD absoluteFrom = baseAddress + from;

         openMemoryAccess(absoluteFrom, 5);
         *(BYTE*)(absoluteFrom) = 0xE9;
         *(DWORD*)(absoluteFrom + 0x1) = relativeAddress;
      } else {
         openMemoryAccess(from, 5);
         *(BYTE*)(from) = 0xE9;
         *(DWORD*)(from + 0x1) = relativeAddress;
      }

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   void writeInterrupt3(const DWORD& to, const int& amount, const bool& isFromAbsolute) {
      EnterCriticalSection(&cs);
      if (!isFromAbsolute) {
         DWORD absoluteTo = baseAddress + to;
         openMemoryAccess(absoluteTo, amount);

         for (int i = 0; i < amount; i++)
            *(BYTE*)(absoluteTo + i) = 0xCC;
      } else {
         openMemoryAccess(to, amount);
         for (int i = 0; i < amount; i++)
            *(BYTE*)(to + i) = 0xCC;
      }

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }
   void writeRet(const DWORD& to, const bool& isToAbsolute) {
      EnterCriticalSection(&cs);
      if (!isToAbsolute) {
         DWORD absoluteTo = baseAddress + to;
         openMemoryAccess(absoluteTo, 1);
         *(BYTE*)(absoluteTo) = 0xC3;
      } else {
         openMemoryAccess(to, 1);
         *(BYTE*)(to) = 0xC3;
      }

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }
   void writeNop(const DWORD& to, const int& amount, const bool& isToAbsolute) {
      EnterCriticalSection(&cs);
      if (!isToAbsolute) {
         DWORD absoluteTo = baseAddress + to;
         openMemoryAccess(absoluteTo, amount);

         for (int i = 0; i < amount; i++)
            *(BYTE*)(absoluteTo + i) = 0x90;
      } else {
         openMemoryAccess(to, amount);
         for (int i = 0; i < amount; i++)
            *(BYTE*)(to + i) = 0x90;
      }

      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }
   void writeRaw(const DWORD& to, const bool& isToAbsolute, const int byteCount, ...) {
      EnterCriticalSection(&cs);
      va_list bytes;
      va_start(bytes, byteCount);

      if (!isToAbsolute) {
         DWORD absoluteTo = baseAddress + to;
         openMemoryAccess(absoluteTo, byteCount);

         for (int i = 0; i < byteCount; i++)
            *(BYTE*)(absoluteTo + i) = va_arg(bytes, BYTE);
      } else {
         openMemoryAccess(to, byteCount);

         for (int i = 0; i < byteCount; i++)
            *(BYTE*)(to + i) = va_arg(bytes, BYTE);
      }

      va_end(bytes);
      restoreMemoryAccess();
      LeaveCriticalSection(&cs);
   }

   void Init() {
      InitializeCriticalSection(&cs);
      baseAddress = (DWORD)GetModuleHandle(0);
   }
}