#pragma once
#include "..\Data\GameTypes.h"
using GameInternals::Data::GameTypes::ObjectData;

namespace GameInternals {
   namespace Gameplay {
      namespace Object {
         static ObjectData** objectDatas = (ObjectData**)(0x5383B0 + 0x400000);

         static bool getObjectData(ObjectData*& out_ObjectData, const int& offset = 0) {
            auto v = objectDatas[offset];
            if (!v) {
               out_ObjectData = nullptr;
               return false;
            }

            out_ObjectData = v;
            return true;
         }
      }
   }
}