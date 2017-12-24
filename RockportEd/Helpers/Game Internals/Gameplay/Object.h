#pragma once
#include <WinDef.h>

namespace GameInternals {
   struct ObjectData {
      float y_Lift;
      float y_Rotation;
      float x_Lift;
      float x_Rotation;

      float y_Position;
      float z_Position;
      float x_Position;

      BYTE unk1;
      BYTE unk2;
      BYTE unk3;
      BYTE unk4;

      float y_Velocity;
      float z_Velocity;
      float x_Velocity;

      float gravity;

      float y_LiftForce;
      float angular_Velocity;
      float x_LiftForce;

      float mass;
   };

   namespace Gameplay {
      namespace Object {
         static DWORD objectData_firstInstance = 0x5386C8 + 0x400000;

         static bool getObjectData(ObjectData*& out_CarData, const int& offset = 0) {
            auto v = (ObjectData*)(objectData_firstInstance + (offset * 0xB0));
            if (!v)
               return false;

            out_CarData = v;
            return true;
         }
      }
   }
}