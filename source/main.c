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

    storage curData;
    gridCode code;

    appState state = APPSTATE_WELCOME;
    menuState menuState = 0;

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
                    miiData mii = selectMii();

                    state = APPSTATE_ENTERPASSCODE;

                    bool exists = readMiiFile(mii.id, &curData);
                    if (exists) menuState |= MENUSTATE_INITPASSCODE;

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
                            curPtr->isSelected = true;
                            break;
                        }
                    }
                }

                if (menuState & MENUSTATE_INITPASSCODE)
                {

                }
                break;
        }

        drawBattery(getBatteryPercentage(), getChargingState());

        endFrame();
    }

    close();
    return 0;
}