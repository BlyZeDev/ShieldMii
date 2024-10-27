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

                    storage data;
                    data.passcodeLength = 6;
                    data.passcode[0] = 1;
                    data.passcode[1] = 2;
                    data.passcode[2] = 4;
                    data.passcode[3] = 5;
                    data.passcode[4] = 8;
                    data.passcode[5] = 12;
                    data.entryCount = 2;
                    data.entries = (entry*)malloc(data.entryCount * sizeof(entry));

                    strncpy(data.entries[0].name, "ExampleName1", MAX_PASSNAME_LENGTH);
                    strncpy(data.entries[0].password, "Password123", MAX_PASSWORD_LENGTH);

                    strncpy(data.entries[1].name, "ExampleName2", MAX_PASSNAME_LENGTH);
                    strncpy(data.entries[1].password, "AnotherPass456", MAX_PASSWORD_LENGTH);

                    Result res = writeMiiFile(mii.id, &data);
                    char error[20];
                    if (R_FAILED(res))
                    {
                        snprintf(error, 20, "1-%ld", R_LEVEL(res));
                        writeError(error);
                        return 0;
                    }

                    storage test;
                    readMiiFile(mii.id, &test);
                    if (R_FAILED(res))
                    {
                        snprintf(error, 20, "2-%ld", R_LEVEL(res));
                        writeError(error);
                        return 0;
                    }
                }

                drawWelcomeScreen();
                break;
        }

        drawBattery(getBatteryPercentage(), getChargingState());

        endFrame();
    }

    close();
    return 0;
}