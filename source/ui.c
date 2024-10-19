#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "util.h"

static C3D_RenderTarget* topPtr = NULL;
static C3D_RenderTarget* botPtr = NULL;

static C2D_TextBuf dynTxtBuf;

void initCitro()
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    topPtr = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    botPtr = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    dynTxtBuf = C2D_TextBufNew(4096);
}

void clearTarget(C3D_RenderTarget* target)
{
    C2D_TargetClear(target, BLACK);
}

void drawGrid(C3D_RenderTarget* target)
{
    C2D_SceneBegin(target);

    const float width = BOTTOM_SCREEN_WIDTH / GRID_SIZE;
    const float height = SCREEN_HEIGHT / GRID_SIZE;
    for (u8 i = 0; i < GRID_SIZE * GRID_SIZE; i++)
    {
        C2D_DrawCircleSolid(width * (i % GRID_SIZE) + width / 2, height * (i / GRID_SIZE) + height / 2, 0, CIRCLE_SIZE, WHITE);
    }
}

void exitCitro()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}