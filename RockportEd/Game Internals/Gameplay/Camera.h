#pragma once
#include "..\..\Memory.h"
#include "..\Data\Data.h"

namespace GameInternals {
#define arrayValue(arrayId) WORD arrayCapacity_##arrayId; WORD arrayCount_##arrayId; WORD arrayItemSizeInBytes_##arrayId; WORD unknown_##arrayId
   struct CameraInfo {
      arrayValue(STIFFNESS);
      float stiffness[2];
      arrayValue(ANGLE);
      float angle[2];
      arrayValue(LAG);
      float lag[2];
      arrayValue(FOV);
      float fov[2];
      arrayValue(HEIGHT);
      float height[2];
      arrayValue(LATEOFFSET);
      float lateOffset[2];
      char** collectionName;
      arrayValue(TILTING);
      bool tilting[2];
      arrayValue(SELECTABLE);
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
         static int getMemoryCameraInfoIndexFromCameraIndex(const int& cameraIndex) {
            switch (cameraIndex) {
               case 0: // Bumper
                  return 0;
               case 1: // Hood
                  return 6;
               case 2: // Close
                  return 1;
               case 3: // Far
                  return 3;
               case 4: // Superfar
                  return 2;
               case 5: // Drift
                  return 5;
               case 6: // Pursuit
                  return 4;
               default:
                  return -1;
            }
         }

         static CameraInfo* getActiveCameraInfoAsPointer() {
            return (CameraInfo*)Memory::readPointer(0x519774, 3, 0x7C, 0x108, 0x0);

            //game only updates this on camera change
            //return (CameraInfo**)Memory::readPointer(0x5B078C, 1, 0x108);
         }
         static bool getActiveCameraInfo(CameraInfo*& out_CameraInfo) {
            auto p = getActiveCameraInfoAsPointer();
            if (!p)
               return false;
            /* handler for old address
            * if (!p || IsBadReadPtr((LPVOID)p, 4) || *(DWORD*)p == 0xDDDDDDDD
            *    || IsBadWritePtr((LPVOID)*p, (UINT_PTR)sizeof(CameraInfo)) || **(DWORD**)p != 0x00020002) { // Yeah yeah, try me.
            *   return false;
            * }
            */

            out_CameraInfo = p;
            return true;
         }

         static bool getCameraDataFromCameraIndex(const int& cameraIndex, CameraInfo*& out_CameraInfo) {
            DWORD* cameraBase = Data::getCameraBase();
            if (!cameraBase)
               return false;

            if (cameraIndex == 1) { // Hood
               return getActiveCameraInfo(out_CameraInfo);
            }
            else {
               // default camera offset: 0x2AF40
               auto val = (CameraInfo*)(*cameraBase + 0x2AFB8 + (getMemoryCameraInfoIndexFromCameraIndex(cameraIndex) * sizeof(CameraInfo)));
               if (!val)
                  return false;

               out_CameraInfo = val;
            }
            return true;
         }
      }
   }
}