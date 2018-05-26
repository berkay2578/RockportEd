#pragma once
#include "..\Data\GameTypes.h"
using namespace GameInternals::Data::GameTypes;

namespace GameInternals {
   namespace Gameplay {
      namespace Object {
         static bool getRigidBodyData(RigidBodyData*& out_RigidBodyData, const int& offset = 0) {
            auto v = RigidBody::mMaps[offset];
            if (!v) {
               out_RigidBodyData = nullptr;
               return false;
            }

            out_RigidBodyData = v;
            return true;
         }
      }
   }
}