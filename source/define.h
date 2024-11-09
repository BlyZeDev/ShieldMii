#ifndef DEFINE_H
#define DEFINE_H

#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60

#define TOP_SCREEN_WIDTH 400
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

#define GRID_SIZE 5
#define GRID_POINTS GRID_SIZE * GRID_SIZE
#define CIRCLE_SIZE 80 / GRID_SIZE

#define MIN_PASSCODE_LENGTH 4

#define MAX_PASSNAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 128

#define PASSCODE_HASH_LENGTH 64

#define BUFFER_COUNT 8

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

typedef enum
{
    ERROR_CODE_NONE,
    ERROR_CODE_PASSCODE_TOO_SHORT,
    ERROR_CODE_PASSCODE_WRONG
} ErrorCode;

typedef enum
{
    APPSTATE_WELCOME = 1,
    APPSTATE_ENTERPASSCODE,
    APPSTATE_MAIN
} AppState;

typedef enum
{
    MENUSTATE_SELECTMII = 1 << 0,
    MENUSTATE_INITPASSCODE = 1 << 1
} MenuState;

typedef struct
{
    u8 hour;
    u8 min;
    u8 sec;
} systemTime;

typedef struct
{
    float x;
    float y;
} point;

typedef struct
{
    point coord;
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

typedef struct
{
    s8* audioBuffer;
    size_t audioSize;
    size_t cursor;
    u32 lastTime;
    bool isLittleEndian;
    bool isLooping;
    u32 infoOffset;
    u32 dataOffset;
    u8 channelCount;
    u32 sampleRate;
    u32 loopStart;
    u32 loopEnd;
    u32 numBlocks;
    u32 blockSize;
    u32 blockSamples;
    u32 lastBlockSamples;
    u32 lastBlockSize;
    u32 currentBlock;
    u16 adpcmCoefs[2][16];
    ndspWaveBuf waveBuf[2][BUFFER_COUNT];
    ndspAdpcmData adpcmData[2][2];
    u16 channel[2];
    u32 activeChannels;
    u8* bufferData[2][BUFFER_COUNT];
    volatile bool stop;
    Thread playingThread;
} bcstm;

typedef struct
{
    u8 batteryPercentage;
    bool chargingState;
    systemTime curTime;
} screenInfo;

#endif