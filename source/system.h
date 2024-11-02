#ifndef SYSTEM_H
#define SYSTEM_H

CFG_SystemModel getSystemModel();
CFG_Language getSystemLang();
miiData selectMii();
void showError(const char* buffer, ErrorCode code);
u8 getBatteryPercentage();
bool getChargingState();

#endif