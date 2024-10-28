#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"
#include "system.h"
#include "util.h"
#include "input.h"
#include "fs.h"
#include "ui.h"

void init()
{
    osSetSpeedupEnable(false); // Set to true when releasing

    romfsInit();
    initFs();

    mcuHwcInit();
    acInit();
    hidInit();
    ptmuInit();

    initUI();
}

void close()
{
    exitUI();

    ptmuExit();
    hidExit();
    acExit();
    mcuHwcInit();

    exitFs();
    romfsInit();
}

int main(int argc, char** argv)
{
    init();

    startFrame();
    drawWelcomeScreen();
    drawBattery(getBatteryPercentage(), getChargingState());
    endFrame();

    storage curData = {0};
    gridCode code = {0};

    appState state = APPSTATE_WELCOME;
    menuState menuState = 0;

    miiData selectedMii;
    while (aptMainLoop())
    {
        updateInput();

        if (kDown & KEY_START) break;

        startFrame();

        switch (state)
        {
            case APPSTATE_WELCOME:
                if (!(menuState & MENUSTATE_SELECTMII))
                {
                    menuState ^= MENUSTATE_SELECTMII;
                    selectedMii = selectMii();

                    state = APPSTATE_ENTERPASSCODE;

                    bool exists = readMiiFile(selectedMii.id, &curData);
                    if (!exists) menuState |= MENUSTATE_INITPASSCODE;

                    initGrid(code.circles);
                }
                break;

            case APPSTATE_ENTERPASSCODE:
                drawGrid(code.circles);

                if (kDown & KEY_TOUCH)
                {
                    circle* curPtr;
                    for (size_t i = 0; i < GRID_POINTS; i++)
                    {
                        curPtr = &code.circles[i];

                        if (curPtr->isSelected) continue;
                        if (isTouched(touchPos, curPtr->x, curPtr->y, CIRCLE_SIZE))
                        {
                            code.code[code.codeLength] = i;
                            code.codeLength++;
                            curPtr->isSelected = true;
                            break;
                        }
                    }
                }

                if (kDown & KEY_A)
                {
                    u8 hashed[200];
                    hash(code.code, code.codeLength, hashed);

                    if (menuState & MENUSTATE_INITPASSCODE)
                    {
                        memcpy(curData.passcodeHash, hashed, sizeof(curData.passcodeHash));
                        writeMiiFile(selectedMii.id, &curData);
                        state = APPSTATE_MAIN;
                    }
                    else
                    {
                        if (memcmp(curData.passcodeHash, hashed, sizeof(curData.passcodeHash)) == 0)
                        {
                            state = APPSTATE_MAIN;
                        }
                        else
                        {
                            memset(&code, 0, sizeof(code));
                            initGrid(code.circles);
                        }
                    }
                }
                break;

            case APPSTATE_MAIN: return 0;
        }

        drawBattery(getBatteryPercentage(), getChargingState());

        endFrame();
    }

    close();
    return 0;
}