#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "util.h"

#define TXT_BUF_SIZE 4096u

static C3D_RenderTarget* topPtr = NULL;
static C3D_RenderTarget* botPtr = NULL;

static C2D_TextBuf dynTxtBuf;
static C2D_SpriteSheet icons;

static void clearTarget(C3D_RenderTarget* target)
{
    C2D_TargetClear(target, ERASER);
}

static point drawText(const char* buffer, const u32 flags, const float x, const float y, const float scaleX, const float scaleY, const u32 color)
{
    C2D_Text text;
    C2D_TextParse(&text, dynTxtBuf, buffer);
    C2D_TextOptimize(&text);
    C2D_DrawText(&text, flags | C2D_WithColor, x, y, 0, scaleX, scaleY, color);

    point size;
    C2D_TextGetDimensions(&text, scaleX, scaleY, &size.x, &size.y);
    return size;
}

void initUI(bool setWide)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    gfxSetWide(setWide);

    topPtr = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    botPtr = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    dynTxtBuf = C2D_TextBufNew(4096);

    icons = C2D_SpriteSheetLoad("romfs:/gfx/icons.t3x");
    if (!icons) svcBreak(USERBREAK_PANIC);
}

void startFrame()
{
    C2D_TextBufClear(dynTxtBuf);
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    clearTarget(topPtr);
    clearTarget(botPtr);
}

void drawWelcomeScreen()
{
    C2D_SceneBegin(topPtr);
    
    C2D_DrawRectSolid(0, 0, 0, TOP_SCREEN_WIDTH, SCREEN_HEIGHT, PRIMARY);

    C2D_Image logo = C2D_SpriteSheetGetImage(icons, 0);
    C2D_DrawImageAt(logo,
        TOP_SCREEN_WIDTH / 2 - logo.subtex->width / 2,
        (SCREEN_HEIGHT - logo.subtex->height) / 2,
        0, NULL, 1.0f, 1.0f);
}

void drawPasscodeEntry(const circle* circles)
{
    C2D_SceneBegin(botPtr);

    circle cur;
    for (size_t i = 0; i < GRID_POINTS; i++)
    {
        cur = circles[i];
        C2D_DrawCircleSolid(cur.coord.x, cur.coord.y, 0, CIRCLE_SIZE, cur.isSelected ? SUCCESS : ERROR);
        C2D_DrawCircleSolid(cur.coord.x, cur.coord.y, 0, CIRCLE_SIZE * 0.75f, TEXT);
    }

    C2D_SceneBegin(topPtr);

    point lastText = drawText("\uE000 Confirm", C2D_AlignCenter, TOP_SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, 1.0f, 1.0f, TEXT);
    lastText = drawText("\uE001 Undo", C2D_AlignCenter, TOP_SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 + lastText.y + CONTROL_PADDING, 1.0f, 1.0f, TEXT);
}

void drawBattery(const u8 percentage, const bool isCharging)
{
    C2D_SceneBegin(topPtr);

    C2D_Image icon = C2D_SpriteSheetGetImage(icons, getBatteryIndex(percentage, isCharging));

    const float xPos = TOP_SCREEN_WIDTH - icon.subtex->width - SCREEN_PADDING;

    C2D_DrawImageAt(icon, xPos, SCREEN_PADDING, 0, NULL, 1.0f, 1.0f);

    char buffer[5];
    snprintf(buffer, 5, "%u%%", percentage);
    drawText(buffer, C2D_AlignRight, xPos - CONTROL_PADDING, SCREEN_PADDING, 0.5f, 0.5f, TEXT);
}

void endFrame()
{
    C2D_Flush();
    C3D_FrameEnd(0);
}

void exitUI()
{
    C2D_TextBufDelete(dynTxtBuf);
    C2D_SpriteSheetFree(icons);

    C2D_Fini();
    C3D_Fini();
    gfxExit();
}