#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
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

    initCitro();
}

void close()
{
    exitCitro();

    hidExit();
    acExit();

    exitFs();
    romfsInit();
}

int main(int argc, char** argv)
{
    init();

    while (aptMainLoop())
    {
        updateInput();

        if (kDown & KEY_START) break;

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        C3D_FrameEnd(0);
    }

    close();
    return 0;
}