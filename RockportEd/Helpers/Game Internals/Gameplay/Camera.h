#pragma once
#include "..\Data\Data.h"
#include "Helpers\Memory\Memory.h"
// Macro for the CameraInfo struct
#define _CAMERA_INFO_ARRAY_DESCRIPTOR(arrayId) WORD arrayCapacity_##arrayId; WORD arrayCount_##arrayId; WORD arrayItemSizeInBytes_##arrayId; WORD unknown_##arrayId;

namespace GameInternals {
   struct CameraInfo {
      _CAMERA_INFO_ARRAY_DESCRIPTOR(STIFFNESS);
      float stiffness[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(ANGLE);
      float angle[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(LAG);
      float lag[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(FOV);
      float fov[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(HEIGHT);
      float height[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(LATEOFFSET);
      float lateOffset[2];
      char* collectionName;
      _CAMERA_INFO_ARRAY_DESCRIPTOR(TILTING);
      bool tilting[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(SELECTABLE);
      bool selectable[2];
   };

   namespace Gameplay {
      namespace Camera {
         static int* getActiveCameraIndexAsPointer() {
            auto pBase = Data::readPointerBase1();
            if (!pBase)
               return nullptr;

            return (int*)(*pBase + 0x8C);
         }
         static int getActiveCameraIndex() {
            int* p = getActiveCameraIndexAsPointer();
            return p ? *p : -1;
         }

         static CameraInfo* getActiveCameraInfoAsPointer() {
            return (CameraInfo*)Memory::readPointer(0x519774, 3, 0x7C, 0x108, 0x0);
         }
         static bool getActiveCameraInfo(CameraInfo*& out_CameraInfo) {
            auto p = getActiveCameraInfoAsPointer();
            if (!p) {
               out_CameraInfo = nullptr;
               return false;
            }

            out_CameraInfo = p;
            return true;
         }
      }
   }
}