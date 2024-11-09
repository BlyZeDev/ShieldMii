#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>

#include "define.h"

CFG_SystemModel getSystemModel(void)
{
    u8 model;
    CFGU_GetSystemModel(&model);

    return model;
}

CFG_Language getSystemLang(void)
{
    u8 language;
    CFGU_GetSystemLanguage(&language);

    return language;
}

miiData selectMii(void)
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
    mii.id = msRet.mii.mii_id;
    miiSelectorReturnGetName(&msRet, mii.name, ARRAY_LENGTH(mii.name));
    mii.birthday_day = msRet.mii.mii_details.bday_day;
    mii.birthday_month = msRet.mii.mii_details.bday_month;

    return mii;
}

void showError(const char* buffer, ErrorCode code)
{
    errorConf conf;
    errorInit(&conf, ERROR_TEXT, CFG_LANGUAGE_DEFAULT);
    errorCode(&conf, code);
    errorText(&conf, buffer);
    errorDisp(&conf);
}

u8 getBatteryPercentage(void)
{
    u8 data[2];
    Result res = MCUHWC_ReadRegister(0xB, data, 2);

    return R_SUCCEEDED(res) ? (u8)roundf(data[0] + data[1] / 256.0f) : 0;
}

bool getChargingState(void)
{
    u8 state;
    PTMU_GetBatteryChargeState(&state);
    return state;
}

systemTime getSystemTime(void)
{
	u64 dayTime = (osGetTime() / 1000) % SECONDS_IN_DAY;

    systemTime curTime;
	curTime.hour = dayTime / SECONDS_IN_HOUR;
	curTime.min = (dayTime % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
	curTime.sec = dayTime % SECONDS_IN_MINUTE;

    return curTime;
}