# RockportEd
An addon to NFS: Most Wanted (2005) with the goal of enhancing the gameplay and adding new general customization options.

## How does it work?
It currently utilizes the [ASI Loader by ThirteenAG](https://github.com/ThirteenAG/Ultimate-ASI-Loader) to be injected to the game memory.
After it is injected, it detours the original DirectInput8Create function and captures all created devices. This makes it possible to have a functioning overlay-UI without interfering with game's own controls.
The UI is drawn with the wonderful [dear imgui](https://github.com/ocornut/imgui).

## Dependencies
This is a strictly WIN32-API library. The code will *not* work in any other environments unless you can provide the necessary libraries. (e.g., WineHQ)
The code utilizes C++14 and is built against the `Windows 7 SDK` with the `Visual Studio 2017 - Windows XP (v141_xp)` toolset. The output will work on Windows XP SP1 and above.

## Development dependencies
If you want to help with the development you will need:
- [June 2010 DirectX SDK](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
- C++14 compliant compiler [e.g., VS2015 and above](https://www.visualstudio.com)
