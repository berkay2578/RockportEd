#pragma once
#include <WinDef.h>

namespace Extensions {
   namespace InGameMenu {
      DWORD WINAPI Init(LPVOID);

      struct PhyicsTuning {
         float steering = 0.0f;
         float handling = 0.0f;
         float brakes = 0.0f;
         float rideheight = 0.0f;
         float aero = 0.0f;
         float nos = 0.0f;
         float turbo = 0.0f;
      };
   }
}