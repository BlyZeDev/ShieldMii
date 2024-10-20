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
static C2D_SpriteSheet icons;

void initUI()
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    topPtr = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    botPtr = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    dynTxtBuf = C2D_TextBufNew(4096);

    icons = C2D_SpriteSheetLoad("romfs:/gfx/icons.t3x");
    if (!icons) svcBreak(USERBREAK_PANIC);
}

void clearTarget(C3D_RenderTarget* target)
{
    C2D_TargetClear(target, ERASE);
}

void drawWelcomeScreen()
{
    clearTarget(topPtr);
    C2D_SceneBegin(topPtr);

    C2D_DrawRectSolid(0, 0, 0, TOP_SCREEN_WIDTH, SCREEN_HEIGHT, INACTIVE);

    C2D_Image logo = C2D_SpriteSheetGetImage(icons, 0);
    C2D_DrawImageAt(logo,
        TOP_SCREEN_WIDTH / 2 - logo.subtex->width / 2,
        (SCREEN_HEIGHT - logo.subtex->height) / 2,
        0, NULL, 1.0f, 1.0f);
}

void initGrid(circle* circles)
{
    const float width = BOTTOM_SCREEN_WIDTH / GRID_SIZE;
    const float height = SCREEN_HEIGHT / GRID_SIZE;

    for (size_t i = 0; i < GRID_POINTS; i++)
    {
        circles[i].x = width * (i % GRID_SIZE) + width / 2;
        circles[i].y = height * (i / GRID_SIZE) + height / 2;
    }
}

void drawGrid(circle* circles)
{
    clearTarget(botPtr);
    C2D_SceneBegin(botPtr);

    for (size_t i = 0; i < GRID_POINTS; i++)
    {
        C2D_DrawCircleSolid(circles[i].x, circles[i].y, 0, CIRCLE_SIZE, SECONDARY);
        C2D_DrawCircleSolid(circles[i].x, circles[i].y, 0, CIRCLE_SIZE * 0.75f, TEXT);
    }
}

void exitUI()
{
    C2D_SpriteSheetFree(icons);

    C2D_Fini();
    C3D_Fini();
    gfxExit();
}