#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "include/sha3.h"

void initGrid(circle* circles)
{
    const float width = BOTTOM_SCREEN_WIDTH / GRID_SIZE;
    const float height = SCREEN_HEIGHT / GRID_SIZE;

    for (size_t i = 0; i < GRID_POINTS; i++)
    {
        circles[i].x = width * (i % GRID_SIZE) + width / 2;
        circles[i].y = height * (i / GRID_SIZE) + height / 2;
        circles[i].isSelected = false;
    }
}

bool isTouched(const touchPosition touch, const float x, const float y, const float area)
{
    float dx = touch.px - x;
    float dy = touch.py - y;
    
    return sqrtf(dx * dx + dy * dy) <= area;
}

size_t getBatteryIndex(const u8 percentage, const bool isCharging)
{
    if (isCharging) return 5;

    if (percentage > 75) return 1;
    else if (percentage > 40) return 2;
    else if (percentage > 10) return 3;
    else return 4;
}

void hash(const u8* input, const size_t length, u8* hashBuf)
{
    sha3_HashBuffer(512, SHA3_FLAGS_KECCAK, input, length, hashBuf, sizeof(hashBuf));
}