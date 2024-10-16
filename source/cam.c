#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"

static const u32 SELECT = SELECT_IN1;
static const u32 PORT = PORT_CAM1;
static const u64 WAIT_TIMEOUT = 300000000;

void initCam()
{
    camInit();

    CAMU_SetSize(SELECT, SIZE_CTR_TOP_LCD, CONTEXT_A);
    CAMU_SetOutputFormat(SELECT, OUTPUT_RGB_565, CONTEXT_A);

    CAMU_SetNoiseFilter(SELECT, true);
    CAMU_SetAutoWhiteBalance(SELECT, true);
    CAMU_SetAutoExposure(SELECT, true);

    CAMU_SetTrimming(PORT, false);
}

void takePicture(u8 *buf)
{
	u32 bufferSize;

	CAMU_GetMaxBytes(&bufferSize, TOP_SCREEN_WIDTH, SCREEN_HEIGHT);
	CAMU_SetTransferBytes(PORT, bufferSize, TOP_SCREEN_WIDTH, SCREEN_HEIGHT);

	CAMU_Activate(SELECT_IN1);

	CAMU_ClearBuffer(PORT);
	CAMU_SetFrameRate(SELECT_IN1, FRAME_RATE_30);

	CAMU_StartCapture(PORT);

    Handle camReceiveEvent;
	CAMU_SetReceiving(&camReceiveEvent, buf, PORT, TOP_SCREEN_SIZE, (s16)bufferSize);
	svcWaitSynchronization(camReceiveEvent, WAIT_TIMEOUT);
	CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_NORMAL);

	CAMU_StopCapture(PORT);

	svcCloseHandle(camReceiveEvent);

	CAMU_Activate(SELECT_NONE);
}

void exitCam()
{
    camExit();
}