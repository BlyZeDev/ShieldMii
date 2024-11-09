#ifndef SYSTEM_H
#define SYSTEM_H

CFG_SystemModel getSystemModel(void);
CFG_Language getSystemLang(void);
miiData selectMii(void);
void showError(const char* buffer, ErrorCode code);
u8 getBatteryPercentage(void);
bool getChargingState(void);
systemTime getSystemTime(void);

#endif