#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "input.h"

#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void init()
{
    osSetSpeedupEnable(false); // Set to true when releasing

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
}

int main(int argc, char** argv)
{
    init();

    while (aptMainLoop())
    {
        updateInput();
    }

    close();
    return 0;
}