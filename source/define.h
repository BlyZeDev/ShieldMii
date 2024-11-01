#ifndef DEFINE_H
#define DEFINE_H

#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define SCREEN_PADDING 2
#define CONTROL_PADDING SCREEN_PADDING * 2

#define ERASER 0xFF000000
#define PRIMARY 0xFF442200
#define SECONDARY 0xFFCCA800
#define INACTIVE 0xFFD0D0D0
#define TEXT 0xFFFFFFFF
#define SUCCESS 0xFF71CC2E
#define ERROR 0xFF3C4CE7

#define GRID_SIZE 4
#define GRID_POINTS GRID_SIZE * GRID_SIZE
#define CIRCLE_SIZE 100 / GRID_SIZE

#define MAX_PASSNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 128

#define PASSCODE_HASH_LENGTH 64

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

typedef enum
{
    APPSTATE_WELCOME = 1,
    APPSTATE_ENTERPASSCODE,
    APPSTATE_MAIN
} appState;

typedef enum
{
    MENUSTATE_SELECTMII = 1 << 0,
    MENUSTATE_INITPASSCODE = 1 << 1
} menuState;

typedef struct
{
    float x;
    float y;
    bool isSelected;
} circle;

typedef struct
{
    circle circles[GRID_POINTS];
    u8 code[GRID_POINTS];
    size_t codeLength;
} gridCode;

typedef struct
{
    u32 id;
    char name[30];
    u8 birthday_day;
    u8 birthday_month;
} miiData;

typedef struct
{
    char name[MAX_PASSNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} entry;

typedef struct
{
    u8 passcodeHash[PASSCODE_HASH_LENGTH];
    u16 entryCount;
    entry* entries;
} storage;

#endif