#ifndef UI_H
#define UI_H

void initUI(bool setWide);
void startFrame(void);
void drawWelcomeScreen(void);
void drawPasscodeEntry(const circle* circles);
void drawBattery(const u8 percentage, const bool isCharging);
void drawClock(const systemTime* timePtr);
void endFrame(void);
void exitUI(void);

#endif