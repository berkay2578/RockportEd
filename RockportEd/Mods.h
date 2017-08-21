#pragma once
#include "stdafx.h"
#include <map>

namespace Mods {
   extern int* activeCamera;
   extern std::map<int, std::map<char*, float*>> cameraData;

   extern UINT frameCount;
   extern bool isRecording;
   extern void startRecording();

   extern bool isShowingReplay;
   extern bool isReplayPaused;
   extern UINT frameNr;
   extern void changeReplayState();
   extern void startReplay();

   DWORD WINAPI Init(LPVOID);
}