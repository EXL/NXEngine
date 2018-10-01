######################################################################
# Written by EXL on Sun Sep 1 11:04:56 2013
######################################################################

# Platform Defines
# -D_480X272        - Use widescreen 480x272 resolution.
# -D_320X240        - Use normal 320x240 resolution.
# -D_DINGUX         - Dingux platform and keyboard layout (Ritmix RZX-50, Dingoo A320, Dingoo A380, etc).
# -D_MOTOMAGX       - MotoMAGX platform and keyboard layout (Motorola ZN5, Z6, E8, EM30, VE66, etc).
# -D_MOTOEZX        - MotoEZX platform and keyboard layout (Motorola E2, E6, A1200, A1600).
# -D_SDL_MIXER      - Don't use generatining music, *.xm and sounds playing via SDL_mixer.
# -D_DEBUG          - Enable detailed debug output to the console.
# -D_L10N_CP1251    - Enable russian localization (need a *.ttf font and russian data-files).

# Global App Settings
TEMPLATE = app
CONFIG -= console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += warn_off
CONFIG -= l10n_rus

# Platform section
# Write here the necessary platform.
# Available platforms (only for linux-mips-g++):
######################
# platform-rzx50 (default)
# platform-a320
######################
linux-mips-g++: CONFIG += platform-rzx50

# Targets
win32-g++ | linux-g++ | linux-g++-64 { # Host
    TARGET = nx
} linux-mips-g++ {
    TARGET = nx.dge
} linux-arm-gnueabi-g++ { # MotoMAGX
    TARGET = nx.magx
} linux-arm-gnu-g++ | iwmmxt_le-g++ { # MotoEZX
    TARGET = nx.ezx
}

# Strip release binary
CONFIG(release, debug|release) {
    !win32-g++: QMAKE_POST_LINK += $(STRIP) $(TARGET)
}

# Defines
win32-g++ | linux-g++ | linux-g++-64 { # Host
    DEFINES +=
} linux-mips-g++ { # Dingux
    CONFIG(platform-rzx50, platform-rzx50 | platform-a320) { # RZX-50
        DEFINES += _480X272 _DINGUX _SDL_MIXER
    } CONFIG(platform-a320, platform-rzx50 | platform-a320) { # A320
        DEFINES += _320X240 _DINGUX _SDL_MIXER
    }
} linux-arm-gnueabi-g++ { # MotoMAGX
    DEFINES += _320X240 _MOTOMAGX _SDL_MIXER
} linux-arm-gnu-g++ | iwmmxt_le-g++ { # MotoEZX
    DEFINES += _320X240 _MOTOEZX _SDL_MIXER
} linux-arm-gnueabi-g++ {
    DEFINES += _BSD_SOURCE
}

# Debug
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
}

# Localization
l10n_rus {
    DEFINES += _L10N_CP1251
}

# MotoEZX sdl-config
EZX_SDL_CONFIG = /opt/toolchains/motoezx/bin/sdl-config

# Platform C/C++ Flags, CPU Optimization
linux-mips-g++ { # Dingux
    QMAKE_CFLAGS_RELEASE += -mabi=32 -msoft-float -ffast-math -G0
    QMAKE_CXXFLAGS_RELEASE += $${QMAKE_CFLAGS_RELEASE}
} linux-arm-gnueabi-g++ { # MotoMAGX
    QMAKE_CFLAGS += $$system(sdl-config  --cflags)
    QMAKE_CXXFLAGS += $${QMAKE_CFLAGS}
    QMAKE_CFLAGS_RELEASE += -march=armv6j -mtune=arm1136jf-s -mfpu=vfp
    QMAKE_CXXFLAGS_RELEASE += $${QMAKE_CFLAGS_RELEASE}
} linux-arm-gnu-g++ | iwmmxt_le-g++ { # MotoEZX
    QMAKE_CFLAGS += $$system($${EZX_SDL_CONFIG}  --cflags)
    QMAKE_CXXFLAGS += $${QMAKE_CFLAGS}
    QMAKE_CFLAGS_RELEASE += -mcpu=iwmmxt -mtune=iwmmxt -march=iwmmxt -marm
    QMAKE_CXXFLAGS_RELEASE += $${QMAKE_CFLAGS_RELEASE}
}

# Headers
INCLUDEPATH += .

win32-g++ { # Host MinGW Windows
    INCLUDEPATH += C:/MinGW/include
    INCLUDEPATH += C:/MinGW/include/SDL
} linux-g++ | linux-g++-64 { # Host GCC Linux
    INCLUDEPATH += /usr/include
    INCLUDEPATH += /usr/include/SDL
} linux-mips-g++ { # Dingux
    INCLUDEPATH += /opt/mipsel-linux-uclibc/usr/include
    INCLUDEPATH += /opt/mipsel-linux-uclibc/usr/include/SDL
} linux-arm-gnueabi-g++ { # MotoMAGX
    INCLUDEPATH += /opt/toolchains/motomagx/arm-eabi2/include
    INCLUDEPATH += /opt/toolchains/motomagx/arm-eabi2/include/SDL
} linux-arm-gnu-g++ { # MotoEZX
    INCLUDEPATH += /opt/toolchains/motoezx/include/
    INCLUDEPATH += /opt/toolchains/motoezx/include/SDL
} haiku {
    INCLUDEPATH += $$system(finddir B_SYSTEM_HEADERS_DIRECTORY)/SDL
}

# Librares
win32-g++ { # Host MinGW Windows
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
    LIBS += -lmingw32 -lSDL_ttf -lfreetype -lpng -lz -lSDLmain -lSDL -lgdi32 -lwinmm
} linux-g++ | linux-g++-64 { # Host GCC Linux
    LIBS += -lSDLmain -lSDL -lSDL_ttf
} linux-mips-g++ { # Dingux
    LIBS += -lSDLmain -lSDL -lSDL_ttf -lSDL_mixer
} linux-arm-gnueabi-g++ { # MotoMAGX
    LIBS += $$system(sdl-config  --libs) -lSDL_mixer -lSDL_ttf -lstdc++ -lm -lfreetype
} linux-arm-gnu-g++ { # MotoEZX
    LIBS += $$system($${EZX_SDL_CONFIG}  --libs) -lSDL_mixer -lSDL_ttf -lstdc++ -lm -lfreetype
} haiku {
    LIBS += $$system(sdl-config  --libs) -lSDL_mixer -lSDL_ttf -lfreetype
}

# Additional MotoEZX files
linux-arm-gnu-g++ | iwmmxt_le-g++ {
    DEPENDPATH += platform/EZX
    HEADERS += platform/EZX/EZX_SDL_PollEvent.h
    SOURCES += platform/EZX/EZX_SDL_PollEvent.cpp
}

# Win32 Icon
win32-g++: RC_FILE = nx.rc

DEPENDPATH += . \
              ai \
              autogen \
              common \
              endgame \
              extract \
              graphics \
              intro \
              pause \
              siflib \
              sound \
              TextBox \
              ai/almond \
              ai/boss \
              ai/egg \
              ai/final_battle \
              ai/first_cave \
              ai/hell \
              ai/last_cave \
              ai/maze \
              ai/npc \
              ai/oside \
              ai/plantation \
              ai/sand \
              ai/sym \
              ai/village \
              ai/weapons \
              ai/weed \
              platform/Linux
# Input
HEADERS += caret.h \
           config.h \
           console.h \
           debug.h \
           dirnames.h \
           floattext.h \
           game.h \
           input.h \
           inventory.h \
           map.h \
           map_system.h \
           maprecord.h \
           nx.h \
           object.h \
           ObjManager.h \
           p_arms.h \
           platform.h \
           player.h \
           profile.h \
           replay.h \
           screeneffect.h \
           settings.h \
           slope.h \
           stageboss.h \
           stagedata.h \
           statusbar.h \
           trig.h \
           tsc.h \
           vararray.h \
           ai/ai.h \
           ai/balrog_common.h \
           ai/IrregularBBox.h \
           ai/stdai.h \
           autogen/asdefs.h \
           autogen/sprites.h \
           common/basics.h \
           common/BList.h \
           common/bufio.h \
           common/DBuffer.h \
           common/DString.h \
           common/FileBuffer.h \
           common/InitList.h \
           common/llist.h \
           common/StringList.h \
           common/SupportDefs.h \
           endgame/credits.h \
           endgame/CredReader.h \
           endgame/island.h \
           graphics/font.h \
           graphics/graphics.h \
           graphics/nxsurface.h \
           graphics/palette.h \
           graphics/safemode.h \
           graphics/sprites.h \
           graphics/tileset.h \
           intro/intro.h \
           intro/title.h \
           pause/dialog.h \
           pause/message.h \
           pause/options.h \
           pause/pause.h \
           siflib/sectSprites.h \
           siflib/sectStringArray.h \
           siflib/sif.h \
           siflib/sifloader.h \
           sound/org.h \
           sound/pxt.h \
           sound/sound.h \
           sound/sslib.h \
           TextBox/ItemImage.h \
           TextBox/SaveSelect.h \
           TextBox/StageSelect.h \
           TextBox/TextBox.h \
           TextBox/YesNoPrompt.h \
           ai/almond/almond.h \
           ai/boss/balfrog.h \
           ai/boss/ballos.h \
           ai/boss/core.h \
           ai/boss/heavypress.h \
           ai/boss/ironhead.h \
           ai/boss/omega.h \
           ai/boss/sisters.h \
           ai/boss/undead_core.h \
           ai/boss/x.h \
           ai/final_battle/doctor.h \
           ai/weapons/weapons.h \
           ai/weapons/whimstar.h \
           common/misc.fdh \
           caret.fdh \
           console.fdh \
           debug.fdh \
           floattext.fdh \
           game.fdh \
           input.fdh \
           inventory.fdh \
           main.fdh \
           map.fdh \
           map_system.fdh \
           niku.fdh \
           object.fdh \
           ObjManager.fdh \
           p_arms.fdh \
           platform.fdh \
           player.fdh \
           playerstats.fdh \
           profile.fdh \
           replay.fdh \
           screeneffect.fdh \
           settings.fdh \
           slope.fdh \
           stageboss.fdh \
           statusbar.fdh \
           trig.fdh \
           tsc.fdh \
           tsc_cmdtbl.h \
           ai/ai.fdh \
           ai/balrog_common.fdh \
           ai/IrregularBBox.fdh \
           common/bufio.fdh \
           common/DBuffer.fdh \
           common/DString.fdh \
           common/InitList.fdh \
           common/StringList.fdh \
           endgame/credits.fdh \
           endgame/CredReader.fdh \
           endgame/island.fdh \
           endgame/misc.fdh \
           extract/crc.fdh \
           extract/extract.fdh \
           extract/extractfiles.fdh \
           extract/extractpxt.fdh \
           extract/extractstages.fdh \
           graphics/font.fdh \
           graphics/graphics.fdh \
           graphics/nxsurface.fdh \
           graphics/palette.fdh \
           graphics/safemode.fdh \
           graphics/sprites.fdh \
           graphics/tileset.fdh \
           intro/intro.fdh \
           intro/title.fdh \
           pause/dialog.fdh \
           pause/message.fdh \
           pause/objects.fdh \
           pause/options.fdh \
           pause/pause.fdh \
           siflib/sectSprites.fdh \
           siflib/sectStringArray.fdh \
           siflib/sif.fdh \
           siflib/sifloader.fdh \
           sound/org.fdh \
           sound/pxt.fdh \
           sound/sound.fdh \
           sound/sslib.fdh \
           TextBox/ItemImage.fdh \
           TextBox/SaveSelect.fdh \
           TextBox/StageSelect.fdh \
           TextBox/TextBox.fdh \
           TextBox/YesNoPrompt.fdh \
           ai/almond/almond.fdh \
           ai/boss/balfrog.fdh \
           ai/boss/ballos.fdh \
           ai/boss/core.fdh \
           ai/boss/heavypress.fdh \
           ai/boss/ironhead.fdh \
           ai/boss/omega.fdh \
           ai/boss/sisters.fdh \
           ai/boss/undead_core.fdh \
           ai/boss/x.fdh \
           ai/weed/weed.fdh \
           ai/egg/egg.fdh \
           ai/egg/egg2.fdh \
           ai/egg/igor.fdh \
           ai/final_battle/balcony.fdh \
           ai/final_battle/doctor.fdh \
           ai/final_battle/doctor_common.fdh \
           ai/final_battle/doctor_frenzied.fdh \
           ai/final_battle/final_misc.fdh \
           ai/final_battle/misery.fdh \
           ai/final_battle/sidekicks.fdh \
           ai/first_cave/first_cave.fdh \
           ai/hell/ballos_misc.fdh \
           ai/hell/ballos_priest.fdh \
           ai/hell/hell.fdh \
           ai/plantation/plantation.fdh \
           ai/sym/sym.fdh \
           ai/last_cave/last_cave.fdh \
           ai/maze/balrog_boss_missiles.fdh \
           ai/maze/critter_purple.fdh \
           ai/maze/gaudi.fdh \
           ai/maze/labyrinth_m.fdh \
           ai/maze/maze.fdh \
           ai/maze/pooh_black.fdh \
           ai/npc/balrog.fdh \
           ai/npc/curly.fdh \
           ai/npc/curly_ai.fdh \
           ai/npc/npcguest.fdh \
           ai/npc/npcplayer.fdh \
           ai/npc/npcregu.fdh \
           ai/sand/sand.fdh \
           ai/oside/oside.fdh \
           ai/sand/puppy.fdh \
           ai/sand/curly_boss.fdh \
           ai/sand/toroko_frenzied.fdh \
           ai/sym/smoke.fdh \
           ai/village/balrog_boss_running.fdh \
           ai/village/ma_pignon.fdh \
           ai/village/village.fdh \
           ai/weapons/blade.fdh \
           ai/weapons/bubbler.fdh \
           ai/weapons/fireball.fdh \
           ai/weapons/missile.fdh \
           ai/weapons/nemesis.fdh \
           ai/weapons/polar_mgun.fdh \
           ai/weapons/snake.fdh \
           ai/weapons/spur.fdh \
           ai/weapons/weapons.fdh \
           ai/weapons/whimstar.fdh \
           ai/weed/balrog_boss_flying.fdh \
           ai/weed/frenzied_mimiga.fdh \
           l10n_strings.h \
           nx_icon.h

SOURCES += caret.cpp \
           console.cpp \
           debug.cpp \
           floattext.cpp \
           game.cpp \
           input.cpp \
           inventory.cpp \
           main.cpp \
           map.cpp \
           map_system.cpp \
           niku.cpp \
           object.cpp \
           ObjManager.cpp \
           p_arms.cpp \
           platform.cpp \
           player.cpp \
           playerstats.cpp \
           profile.cpp \
           replay.cpp \
           screeneffect.cpp \
           settings.cpp \
           slope.cpp \
           stageboss.cpp \
           stagedata.cpp \
           statusbar.cpp \
           trig.cpp \
           tsc.cpp \
           ai/ai.cpp \
           ai/balrog_common.cpp \
           ai/IrregularBBox.cpp \
           autogen/AssignSprites.cpp \
           autogen/objnames.cpp \
           common/BList.cpp \
           common/bufio.cpp \
           common/DBuffer.cpp \
           common/DString.cpp \
           common/FileBuffer.cpp \
           common/InitList.cpp \
           common/misc_comm.cpp \
           common/stat.cpp \
           common/StringList.cpp \
           endgame/credits.cpp \
           endgame/CredReader.cpp \
           endgame/island.cpp \
           endgame/misc_end.cpp \
           extract/crc.cpp \
           extract/extract.cpp \
           extract/extractfiles.cpp \
           extract/extractpxt.cpp \
           extract/extractstages.cpp \
           graphics/font.cpp \
           graphics/graphics.cpp \
           graphics/nxsurface.cpp \
           graphics/palette.cpp \
           graphics/safemode.cpp \
           graphics/sprites.cpp \
           graphics/tileset.cpp \
           intro/intro.cpp \
           intro/title.cpp \
           pause/dialog.cpp \
           pause/message.cpp \
           pause/objects.cpp \
           pause/options.cpp \
           pause/pause.cpp \
           siflib/sectSprites.cpp \
           siflib/sectStringArray.cpp \
           siflib/sif.cpp \
           siflib/sifloader.cpp \
           sound/org.cpp \
           sound/pxt.cpp \
           sound/sound.cpp \
           sound/sslib.cpp \
           TextBox/ItemImage.cpp \
           TextBox/SaveSelect.cpp \
           TextBox/StageSelect.cpp \
           TextBox/TextBox.cpp \
           TextBox/YesNoPrompt.cpp \
           ai/almond/almond.cpp \
           ai/boss/balfrog.cpp \
           ai/boss/ballos.cpp \
           ai/boss/core.cpp \
           ai/boss/heavypress.cpp \
           ai/boss/ironhead.cpp \
           ai/boss/omega.cpp \
           ai/boss/sisters.cpp \
           ai/boss/undead_core.cpp \
           ai/boss/x.cpp \
           ai/egg/egg.cpp \
           ai/egg/egg2.cpp \
           ai/egg/igor.cpp \
           ai/final_battle/balcony.cpp \
           ai/final_battle/doctor.cpp \
           ai/final_battle/doctor_common.cpp \
           ai/final_battle/doctor_frenzied.cpp \
           ai/final_battle/final_misc.cpp \
           ai/final_battle/misery_battle.cpp \
           ai/final_battle/sidekicks.cpp \
           ai/first_cave/first_cave.cpp \
           ai/hell/ballos_misc.cpp \
           ai/hell/ballos_priest.cpp \
           ai/hell/hell.cpp \
           ai/last_cave/last_cave.cpp \
           ai/maze/balrog_boss_missiles.cpp \
           ai/maze/critter_purple.cpp \
           ai/maze/gaudi.cpp \
           ai/maze/labyrinth_m.cpp \
           ai/maze/maze.cpp \
           ai/maze/pooh_black.cpp \
           ai/npc/balrog.cpp \
           ai/npc/curly.cpp \
           ai/npc/curly_ai.cpp \
           ai/npc/misery_ai.cpp \
           ai/npc/npcguest.cpp \
           ai/npc/npcplayer.cpp \
           ai/npc/npcregu.cpp \
           ai/oside/oside.cpp \
           ai/plantation/plantation.cpp \
           ai/sand/curly_boss.cpp \
           ai/sand/puppy.cpp \
           ai/sand/sand.cpp \
           ai/sand/toroko_frenzied.cpp \
           ai/sym/smoke.cpp \
           ai/sym/sym.cpp \
           ai/village/balrog_boss_running.cpp \
           ai/village/ma_pignon.cpp \
           ai/village/village.cpp \
           ai/weapons/blade.cpp \
           ai/weapons/bubbler.cpp \
           ai/weapons/fireball.cpp \
           ai/weapons/missile.cpp \
           ai/weapons/nemesis.cpp \
           ai/weapons/polar_mgun.cpp \
           ai/weapons/snake.cpp \
           ai/weapons/spur.cpp \
           ai/weapons/weapons.cpp \
           ai/weapons/whimstar.cpp \
           ai/weed/balrog_boss_flying.cpp \
           ai/weed/frenzied_mimiga.cpp \
           ai/weed/weed.cpp \
           platform/Linux/vbesync.c

OTHER_FILES += nx.rc \
               README.md
