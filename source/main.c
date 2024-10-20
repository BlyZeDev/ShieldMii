#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "system.h"
#include "util.h"
#include "input.h"
#include "fs.h"
#include "ui.h"

void init()
{
    osSetSpeedupEnable(false); // Set to true when releasing

    romfsInit();
    initFs();

    acInit();
    hidInit();
    ptmuInit();

    initUI();
}

void close()
{
    exitUI();

    ptmuExit();
    hidExit();
    acExit();

    exitFs();
    romfsInit();
}

int main(int argc, char** argv)
{
    init();

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    drawWelcomeScreen();
    C3D_FrameEnd(0);

    miiData mii = selectMii();

    while (aptMainLoop())
    {
        updateInput();

        if (kDown & KEY_START) break;

        
    }

    close();
    return 0;
}