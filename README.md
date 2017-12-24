# RockportEd
An addon to NFS: Most Wanted (2005) with the goal of enhancing the gameplay and adding new general customization options.

## How to make it work?
1. Get [MirrorHook](https://github.com/berkay2578/MirrorHook).
2. Get an ASI Loader like [the one by ThirteenAG](https://github.com/ThirteenAG/Ultimate-ASI-Loader) and get it to load MirrorHook and RockportEd. (or inject them to the process manually.)
3. Voila. After MirrorHook initializes itself, RockportEd will show itself using the wonderful [dear imgui](https://github.com/ocornut/imgui).

## Development dependencies
- [June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- [MirrorHook](https://github.com/berkay2578/MirrorHook)
- C++14 compliant compiler [e.g., VS2015 and above](https://www.visualstudio.com)
- Universal CRT SDK

## How to set up
- Set up MirrorHook first.
- Change the `MIRRORHOOK_DEFINITIONS_PATH` inside `stdafx.h`.
- Change the parameters of the `copy` command in Post-Build Event or remove it.
- Compile.

## Notes
This is a strictly WIN32-API library. The code will *not* work in any other environment unless you can provide the necessary libraries. (e.g., WineHQ)
The code utilizes C++14 and is built against the `Windows 7 SDK` with the `Visual Studio 2017 - Windows XP (v141_xp)` toolset. The output will work on Windows XP SP1 and above.
