#pragma once
#include "..\Data\Data.h"

namespace GameInternals {
   struct CameraData {
      float horizontalAngle;
      DWORD unk1, unk2, unk3;
      float posX;
      DWORD unk4, unk5, unk6;
      float fov;
      DWORD unk7, unk8, unk9;
      float posZ;
      DWORD unk10, unk11, unk12;
      float verticalAngle;
   };

   namespace Gameplay {
      namespace Camera {
         static int* getActiveCameraIdAsPointer() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return nullptr;

            return (int*)(*pBase + 0x8C);
         }

         static int getActiveCameraId() {
            int* p = getActiveCameraIdAsPointer();
            return p ? *p : -1;
         }

         static bool getCameraDataById(const int& cameraId, CameraData*& out_CameraData) {
            DWORD* cameraBase = Data::getCameraBase();
            if (!cameraBase)
               return false;

            switch (cameraId) {
               case 0: // Bumper Camera
                  out_CameraData = (CameraData*)(*cameraBase + 0x2AFD0);
                  break;
               case 2: // Near Camera
                  out_CameraData = (CameraData*)(*cameraBase + 0x2B048);
                  break;
               case 3: // Far Camera
                  out_CameraData = (CameraData*)(*cameraBase + 0x2B138);
                  break;
               default:
                  return false;
            }
            return true;
         }
      }
   }
}