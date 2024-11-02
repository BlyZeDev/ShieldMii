#ifndef UI_H
#define UI_H

void initUI(bool setWide);
void startFrame();
void drawWelcomeScreen();
void drawPasscodeEntry(const circle* circles);
void drawBattery(const u8 percentage, const bool isCharging);
void endFrame();
void exitUI();

#endif