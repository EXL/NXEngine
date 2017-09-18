NXEngine
========

NXEngine is open-source rewrite engine of the [Cave Story](https://en.wikipedia.org/wiki/Cave_Story) game. Author -- Caitlin Shaw (rogueeve)

This project is my NXEngine fork for weak devices with some improvements:

1. Sound through SDL_mixer;

2. Support for widescreen displays (480x272);

3. Russification.

## Downloads:

You can download ready-to-run packages with NXEngine at this link (English and Russian versions):

[Download NXEngine Packages](http://exlmoto.ru/nxengine/#4 "Download NXEngine Packages").

## Building:

Building NXEngine requires some SDL libraries: SDL-1.2, SDL_ttf-2.0, SDL_mixer-1.2 (optional). Please install these libraries and their header files before building the project. Please read "nx.pro" file before compiling NXEngine.

Platform Defines:

| Define | Description |
| --- | --- |
| `-D_480X272` | Use widescreen 480x272 resolution. |
| `-D_320X240` | Use normal 320x240 resolution. |
| `-D_DINGUX` | Dingux platform and keyboard layout (Ritmix RZX-50, Dingoo A320, Dingoo A380, etc). |
| `-D_MOTOMAGX` | MotoMAGX platform and keyboard layout (Motorola ZN5, Z6, E8, EM30, VE66, etc). |
| `-D_MOTOEZX` | MotoEZX platform and keyboard layout (Motorola E2, E6, A1200, A1600). |
| `-D_SDL_MIXER` | Don't use music generation, `*.xm` tracks and sounds will be played via SDL_mixer. |
| `-D_DEBUG` | Enable detailed debug output to the console. |
| `-D_L10N_CP1251` | Enable Russian l10n (you need a `*.ttf` font and Russian data-files). |

[Addition building information (in Russian)](http://exlmoto.ru/nxengine/#3 "Addition building information (in Russian)").

## Screens:

Ritmix RZX-50:

![Ritmix RZX-50](screens/Cave_Story_Ritmix_1.png)

![Ritmix RZX-50](screens/Cave_Story_Ritmix_2.png)

Windows 8.1:

![Windows 8.1](screens/Cave_Story_Windows.png)

## Photos:

![Motorola ROKR E6](platform/EZX/NXEngine-EZX-ROKR_E6.jpg)

## Videos:

Demonstration [video on YouTube](https://youtu.be/aZPgX9Ismq4) of the launch Cave Story game with this engine on the Ritmix RZX-50 device.
