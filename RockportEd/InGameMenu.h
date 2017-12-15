#pragma once
#include "_BaseInGameMenuItem.hpp"

namespace Extensions {
   namespace InGameMenu {
      extern std::vector<_BaseInGameMenuItem*> items;
      void Init();

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