#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

bool isTouched(touchPosition touch, float x, float y, float area)
{
    float dx = touch.px - x;
    float dy = touch.py - y;
    
    return sqrtf(dx * dx + dy * dy) <= area;
}

size_t getBatteryIndex(u8 percentage, bool isCharging)
{
    if (isCharging) return 5;

    if (percentage > 75) return 1;
    else if (percentage > 40) return 2;
    else if (percentage > 10) return 3;
    else return 4;
}