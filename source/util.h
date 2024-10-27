#ifndef UTIL_H
#define UTIL_H

void initGrid(circle* circles);
bool isTouched(const touchPosition touch, const float x, const float y, const float area);
size_t getBatteryIndex(const u8 percentage, const bool isCharging);
void hash(const u8* input, const size_t length, u8* hashBuf);

#endif