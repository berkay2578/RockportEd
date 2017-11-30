#pragma once
#include "stdafx.h"
#include "FixAnnoyances.hpp"

namespace Extensions {
   namespace WndProc {
      static DWORD WINAPI InitAll(LPVOID) {
         FixAnnoyances::Init();

         return TRUE;
      }
   }
}