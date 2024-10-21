#ifndef UTIL_H
#define UTIL_H

bool isTouched(touchPosition touch, float x, float y, float area);
size_t getBatteryIndex(u8 percentage, bool isCharging);

#endif