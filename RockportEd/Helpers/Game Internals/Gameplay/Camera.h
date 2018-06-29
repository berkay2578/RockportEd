/*
   MIT License
   
   Copyright (c) Berkay Yigit
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#pragma once
#include "..\Data\Data.h"
#include "Helpers\Memory\Memory.h"
// Macro for the CameraInfo struct
#define _CAMERA_INFO_ARRAY_DESCRIPTOR(arrayId) WORD arrayCapacity_##arrayId; WORD arrayCount_##arrayId; WORD arrayItemSizeInBytes_##arrayId; WORD unknown_##arrayId;

namespace GameInternals {
   struct CameraInfo {
      _CAMERA_INFO_ARRAY_DESCRIPTOR(STIFFNESS);
      float Stiffness[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(ANGLE);
      float Angle[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(LAG);
      float Lag[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(FOV);
      float FOV[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(HEIGHT);
      float Height[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(LATEOFFSET);
      float LateOffset[2];
      char* CollectionName;
      _CAMERA_INFO_ARRAY_DESCRIPTOR(TILTING);
      bool Tilting[2];
      _CAMERA_INFO_ARRAY_DESCRIPTOR(SELECTABLE);
      bool Selectable[2];

      void operator=(const CameraInfo& newCameraInfo) {
         memcpy_s(Stiffness, sizeof(float) * 2, newCameraInfo.Stiffness, sizeof(float) * 2);
         memcpy_s(Angle, sizeof(float) * 2, newCameraInfo.Angle, sizeof(float) * 2);
         memcpy_s(Lag, sizeof(float) * 2, newCameraInfo.Lag, sizeof(float) * 2);
         memcpy_s(FOV, sizeof(float) * 2, newCameraInfo.FOV, sizeof(float) * 2);
         memcpy_s(Height, sizeof(float) * 2, newCameraInfo.Height, sizeof(float) * 2);
         memcpy_s(LateOffset, sizeof(float) * 2, newCameraInfo.LateOffset, sizeof(float) * 2);
         memcpy_s(Tilting, sizeof(bool) * 2, newCameraInfo.Tilting, sizeof(bool) * 2);
         memcpy_s(Selectable, sizeof(bool) * 2, newCameraInfo.Selectable, sizeof(bool) * 2);
      }
      void setTo(CameraInfo* pNewCameraInfo) {
         *this = *pNewCameraInfo;
      }
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
            return (CameraInfo*)Memory::readPointer(0x519774, false, 3, 0x7C, 0x108, 0x0);
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
} namespace CameraInternals = GameInternals::Gameplay::Camera;