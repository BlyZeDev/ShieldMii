#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "audio.h"
#include "system.h"
#include "util.h"
#include "input.h"
#include "fs.h"
#include "ui.h"

static void updateOverlay(screenOverlay* overlayPtr)
{
    overlayPtr->batteryPercentage = getBatteryPercentage();
    overlayPtr->chargingState = getChargingState();
    overlayPtr->curTime = getSystemTime();
}

static void init(void)
{
    osSetSpeedupEnable(false); // Set to true when releasing

    romfsInit();
    initFs();

    mcuHwcInit();
    cfguInit();
    acInit();
    hidInit();
    ptmuInit();
    ndspInit();

    ndspSetOutputMode(NDSP_OUTPUT_SURROUND);

    initUI(getSystemModel() != CFG_MODEL_2DS);
}

static void close(void)
{
    exitUI();

    ndspExit();
    ptmuExit();
    hidExit();
    acExit();
    cfguExit();
    mcuHwcInit();
    
    exitFs();
    romfsInit();
}

int main(void)
{
    init();

    bcstm* bgm = loadBcstm("romfs:/bgm.bcstm");
    if (bgm != NULL)
    {
        playBcstm(bgm);
    }

    screenOverlay overlay = {0};
    updateOverlay(&overlay);

    startFrame();
    drawWelcomeScreen();
    endFrame();

    storage curData = {0};
    gridCode code = {0};

    AppState appState = APPSTATE_WELCOME;
    MenuState menuState = MENUSTATE_SELECTMII;

    miiData selectedMii;

    u32 frames = 0;
    while (aptMainLoop())
    {
        updateInput();

        if (kDown & KEY_START) break;

        startFrame();
        frames++;

        switch (appState)
        {
            case APPSTATE_WELCOME:
                if (menuState & MENUSTATE_SELECTMII)
                {
                    menuState ^= MENUSTATE_SELECTMII;
                    selectedMii = selectMii();

                    appState = APPSTATE_ENTERPASSCODE;

                    bool exists = readMiiFile(selectedMii.id, &curData);
                    if (!exists) menuState |= MENUSTATE_INITPASSCODE;

                    initGrid(code.circles);
                }
                break;

            case APPSTATE_ENTERPASSCODE:
                drawPasscodeEntry(code.circles);

                if (kDown & KEY_TOUCH)
                {
                    circle* curCirclePtr;
                    for (size_t i = 0; i < GRID_POINTS; i++)
                    {
                        curCirclePtr = &code.circles[i];

                        if (curCirclePtr->isSelected) continue;
                        if (isTouched(touchPos, curCirclePtr->coord.x, curCirclePtr->coord.y, CIRCLE_SIZE))
                        {
                            code.code[code.codeLength] = i;
                            code.codeLength++;
                            curCirclePtr->isSelected = true;
                            break;
                        }
                    }
                }

                if (kDown & KEY_B)
                {
                    if (code.codeLength > 0)
                    {
                        code.codeLength--;
                        code.circles[code.code[code.codeLength]].isSelected = false;
                    }
                }

                if (kDown & KEY_A)
                {
                    if (code.codeLength < GRID_SIZE)
                    {
                        showError("The passcode is too short!", ERROR_CODE_PASSCODE_TOO_SHORT);
                    }
                    else
                    {
                        u8 hashed[PASSCODE_HASH_LENGTH];
                        hash(code.code, code.codeLength, hashed);

                        if (menuState & MENUSTATE_INITPASSCODE)
                        {
                            memcpy(curData.passcodeHash, hashed, sizeof(curData.passcodeHash));
                            writeMiiFile(selectedMii.id, &curData);
                            menuState ^= MENUSTATE_INITPASSCODE;
                            appState = APPSTATE_MAIN;
                        }
                        else
                        {
                            if (memcmp(curData.passcodeHash, hashed, sizeof(curData.passcodeHash)) == 0)
                            {
                                appState = APPSTATE_MAIN;
                            }
                            else
                            {
                                memset(&code, 0, sizeof(code));
                                initGrid(code.circles);
                                showError("The passcode is wrong!", ERROR_CODE_PASSCODE_WRONG);
                            }
                        }
                    }
                }
                break;

            case APPSTATE_MAIN: appState = APPSTATE_TERMINATE; break;

            case APPSTATE_TERMINATE: break;
        }

        if (frames % FPS == 0)
        {
            updateOverlay(&overlay);
        }

        drawOverlay(&overlay);

        endFrame();

        if (appState == APPSTATE_TERMINATE) break;
    }
    
    stopBcstm(bgm);
    freeBcstm(&bgm);

    close();
    return 0;
}