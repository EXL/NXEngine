NXEngine
=============

NXEngine is open-source rewrite engine of the Cave Story. Author - Caitlin Shaw (rogueeve)

Added:

1. Sound through SDL_mixer;

2. Support for widescreen displays (480x272);

3. Russification.

## Building:

Building NXEngine requires next SDL libraries: SDL-1.2, SDL_ttf-2.0, SDL_mixer-1.2 (optional).

Please install these libraries before building the project.

Please read "nx.pro" file before compiling NXEngine.

Platform Defines:

-D_480X272        - Use widescreen 480x272 resolution.

-D_320X240        - Use normal 320x240 resolution.

-D_DINGUX         - Dingux platform and keyboard layout (Ritmix RZX-50, Dingoo A320, Dingoo A380, etc).

-D_MOTOMAGX       - MotoMAGX platform and keyboard layout (Motorola ZN5, Z6, E8, EM30, VE66, etc).

-D_MOTOEZX        - MotoEZX platform and keyboard layout (Motorola E2, E6, A1200, A1600).

-D_SDL_MIXER      - Don't use generatining music, *.xm and sounds playing via SDL_mixer.

-D_DEBUG          - Enable detailed debug output to the console.

-D_L10N_CP1251    - Enable russian localization (need a *.ttf font and russian data-files).

[More building information](http://exlmoto.ru/nxengine/#3 "More building information")

##Screens:

Ritmix RZX-50:

![Ritmix RZX-50](https://raw.github.com/EXL/NXEngine/master/screens/Cave_Story_Ritmix_1.png)

![Ritmix RZX-50](https://raw.github.com/EXL/NXEngine/master/screens/Cave_Story_Ritmix_2.png)

Windows 8.1:

![Windows 8.1](https://raw.github.com/EXL/NXEngine/master/screens/Cave_Story_Windows.png)

##Photos:
![Motorola ROKR E6](https://raw.github.com/EXL/NXEngine/master/platform/EZX/NXEngine-EZX-ROKR_E6.jpg)

##Binares:

Download ready-to-run packages with NXEngine you can here:

[Download NXEngine binares](http://exlmoto.ru/nxengine/#4 "Download NXEngine binares")
