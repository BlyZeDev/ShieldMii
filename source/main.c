#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "input.h"
#include "fs.h"

void init()
{
    osSetSpeedupEnable(false); // Set to true when releasing

    initFs();
    acInit();
    hidInit();

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
}

void close()
{
    C2D_Fini();
    C3D_Fini();

    hidExit();
    acExit();
    exitFs();
}

int main(int argc, char** argv)
{
    init();

    writeError("This is a test error");

    close();
    return 0;

    gfxInit(GSP_RGBA8_OES, GSP_RGBA8_OES, false);

    while (aptMainLoop())
    {
        updateInput();

        if (kDown & KEY_START) break;
    }

    gfxExit();

    while (aptMainLoop())
    {
        updateInput();

        if (kDown & KEY_START) break;
    }

    close();
    return 0;
}