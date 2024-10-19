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