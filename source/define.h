#ifndef DEFINE_H
#define DEFINE_H

#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define ERASE 0xFF000000
#define PRIMARY 0xFF442200
#define SECONDARY 0xFFCCA800
#define INACTIVE 0xFFD0D0D0
#define TEXT 0xFFFFFFFF
#define SUCCESS 0xFF71CC2E
#define ERROR 0xFF3C4CE7

#define GRID_SIZE 4
#define GRID_POINTS GRID_SIZE * GRID_SIZE
#define CIRCLE_SIZE 46 / GRID_SIZE

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

typedef struct
{
    float x;
    float y;
} circle;

typedef struct
{
    circle circles[GRID_POINTS];
    u8 code[GRID_POINTS];
    size_t codeLength;
} gridCode;

typedef struct
{
    char name[30];
    u8 birthday_day;
    u8 birthday_month;
} miiData;

#endif