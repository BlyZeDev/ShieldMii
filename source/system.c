#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"

miiData selectMii()
{
    MiiSelectorConf msConf;
    MiiSelectorReturn msRet;

    miiSelectorInit(&msConf);
    miiSelectorSetTitle(&msConf, "Select a Mii");
    miiSelectorSetOptions(&msConf, 0);
    miiSelectorSetInitialIndex(&msConf, 0);
    miiSelectorLaunch(&msConf, &msRet);

    if (!miiSelectorChecksumIsValid(&msRet)) svcBreak(USERBREAK_PANIC);

    miiData mii;
    miiSelectorReturnGetName(&msRet, mii.name, ARRAY_LENGTH(mii.name));
    mii.birthday_day = msRet.mii.mii_details.bday_day;
    mii.birthday_month = msRet.mii.mii_details.bday_month;

    return mii;
}

u8 getBatteryLevel()
{
    u8 level;
    PTMU_GetBatteryLevel(&level);
    return level;
}

bool getChargingState()
{
    u8 state;
    PTMU_GetBatteryChargeState(&state);
    return state;
}