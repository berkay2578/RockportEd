#pragma once
// Win32 targeting
#include <SDKDDKVer.h>
// Win32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// ¿Things?
#define _CRT_NO_SECURE_WARNINGS
// Commonly used headers
#include <map>
#include <string>
#include <vector>
#include "Memory.h"
// Project-wide macros
#define MIRRORHOOK_DEFINITIONS_PATH "..\..\MirrorHook\MirrorHook\inc\Definitions.hpp"